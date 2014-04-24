#include "ClientSimulator.hpp"
#include "NetworkCodes.hpp"
#include <iostream>

static const sf::Time SELECTOR_WAIT_TIME = sf::seconds(0.2f);
static const sf::Time CONNECTION_MAX_TIME = sf::seconds(7.f);
static const sf::Time CONNECTION_INFO_MAX_TIME = sf::seconds(10.f);

//TODO: Check each received value (e.g. Does the id exist ? Is it different of NO_CHARACTER_ID ?)

ClientSimulator::ClientSimulator():
	GameSimulator(false),
	m_thread(nullptr),
	m_thrrunning(false),
	m_udpmgr(*this)
{

}

ClientSimulator::~ClientSimulator()
{
	stopNetThread();
}

bool ClientSimulator::update(float etime)
{
	//If we're not connected anymore, simulation is over
	if(!isConnected())
		return false;

	//Treat packets
	bool success = true;
	m_receivedpackets.foreach([this, &success](sf::Packet *&packet)
	{
		if(success)
			success = parseReceivedPacket(*packet);
		delete packet;
	});
	m_receivedpackets.clear();

	if(!success)
		return false;
	if(!m_udpmgr.update(etime))
		return false;
	return GameSimulator::update(etime);
}

int ClientSimulator::startNetThread(const sf::IpAddress &serveraddr, unsigned short tcpport, unsigned short udpport, const std::string &name)
{
	//If the thread already exists, don't start a new one
	if(m_thread)
		return -1;

	sf::Socket::Status status;
	//Try starting the UDP server
	unsigned short localudpport = m_udpmgr.startNetThread(serveraddr, udpport);
	if(localudpport == 0)
	{
		std::cerr << "Cannot start UDP networking thread." << std::endl;
		m_server.disconnect();
		return -1;
	}
	//Connect to the server
	m_server.setBlocking(true);
	if((status = m_server.connect(serveraddr, tcpport, CONNECTION_MAX_TIME)) != sf::Socket::Done)
		return -1;
	//Send connection info
	sf::Packet packet;
	packet << name << localudpport;
	if((status = m_server.send(packet)) != sf::Socket::Done)
	{
		std::cerr << "Error while sending connection informations." << std::endl;
		m_server.disconnect();
		return -1;
	}
	//Receive connection reply
	m_server.setBlocking(false);
	sf::SocketSelector selector;
	m_server.addTo(selector);
	sf::Clock clock;//Used to avoid false positive by the selector (happens sometime)
	while(clock.getElapsedTime() <= CONNECTION_INFO_MAX_TIME)
	{
		selector.wait(CONNECTION_INFO_MAX_TIME - clock.getElapsedTime());
		if((status = m_server.receive(packet)) != sf::Socket::NotReady)
			break;
	}
	//Analyse the received packet
	if(status != sf::Socket::Done)
	{
		if(status == sf::Socket::NotReady)
			std::cerr << "Connection timed out while receiving connection data." << std::endl;
		else if(status == sf::Socket::Disconnected)
			std::cerr << "Disconnected while receiving connection data." << std::endl;
		else
			std::cerr << "Unexpected error while receiving connection data." << std::endl;
		return -1;
	}
	//Extract the connection status (accepted ?)
	sf::Uint8 connectionstatus;
	if(!(packet >> connectionstatus))
	{
		std::cerr << "Invalid connection data." << std::endl;
		m_server.disconnect();
		return -1;
	}
	if(connectionstatus != (sf::Uint8)ConnectionStatus::Accepted)
	{
		m_server.disconnect();
		return connectionstatus;
	}
	//Connection accepted, parse the connection data
	if(!parseConnectionData(packet))
	{
		std::cerr << "Invalid connection data." << std::endl;
		m_server.disconnect();
		return -1;
	}

	//Create the thread
	try
	{
		m_thrrunning = true;
		m_thread = new std::thread(&ClientSimulator::netThread, this);
	}
	catch(const std::exception &e)
	{
		m_thrrunning = false;
		std::cerr << "Could not start networking thread." << std::endl;
		std::cerr << e.what() << std::endl;
		delete m_thread;
		m_thread = nullptr;
		m_server.disconnect();
		m_udpmgr.stopNetThread();
		return -1;
	}

	return (int)ConnectionStatus::Accepted;
}

void ClientSimulator::stopNetThread()
{
	//If the thread exists
	if(m_thread)
	{
		m_thrrunning = false;

		//Wait for the thread to stop
		try
		{
			m_thread->join();
		}
		catch(const std::system_error &e)
		{
			std::cerr << "Could not stop networking thread." << std::endl;
			std::cerr << e.what() << std::endl;
		}

		//Delete the thread object
		delete m_thread;
		m_thread = nullptr;

		//Delete the remaining packets
		m_receivedpackets.foreach([](sf::Packet *&packet)
		{
			delete packet;
		});
		m_receivedpackets.clear();
	}
}

bool ClientSimulator::isConnected() const
{
	return m_thrrunning;
}

void ClientSimulator::selfSetDirection(const sf::Vector2f &direction)
{
	//Only tell the server we want to move, don't start to move until the server replies
	sf::Packet packet;
	packet << (sf::Uint8)PacketType::SetDirection << direction.x << direction.y;
	m_server.send(packet);
}

bool ClientSimulator::onSnapshotReceived(sf::Packet &packet)
{
	//How many characters ?
	sf::Uint16 characterscount;
	if(!(packet >> characterscount))
	{
		std::cerr << "Error in snapshot packet1." << std::endl;
		return false;
	}
	//Extract all characters info
	sf::Uint16 charid;
	float x, y;
	for(sf::Uint16 i = 0; i < characterscount; i++)
	{
		if(!(packet >> charid >> x >> y))
		{
			std::cerr << "Error in snapshot packet2." << std::endl;
			return false;
		}
		try
		{
			//Apply modifications to character if it exists
			getCharacter(charid).setPosition(x, y);
		}
		catch(const std::out_of_range &e)
		{

		}
	}
	return true;
}

bool ClientSimulator::parseConnectionData(sf::Packet &packet)
{
	sf::Uint8 playerscount;
	if(!(packet >> playerscount >> m_ownid))
		return false;

	//Add all the players
	Player player;
	for(sf::Uint8 i = 0; i < playerscount; i++)
	{
		if(!(packet >> player))
			return false;
		if(!addPlayer(std::move(player)))
			return false;
	}
	if(!playerExists(m_ownid))
		return false;
	//Get the map
	std::string mapname;
	if(!(packet >> mapname))
		return false;
	if(!loadMap(mapname))
		return false;
	//Get the characters
	sf::Uint16 characterscount;
	if(!(packet >> characterscount))
		return false;
	Character character;
	for(sf::Uint16 i = 0; i < characterscount; i++)
	{
		if(!character.loadFromPacket(packet))
			return false;
		character.setFullySimulated(false);
		character.setInterpolationTime(DEFAULT_INTERPOLATION_TIME);
		if(!addCharacter(std::move(character)))
			return false;
	}
	sf::Uint16 charid;
	packet >> charid;
	if(charid != NO_CHARACTER_ID)
	{
		if(!setOwnCharacter(charid))
			return false;
	}
	return true;
}

bool ClientSimulator::parseReceivedPacket(sf::Packet &packet)
{
	sf::Uint8 type;
	if(!(packet >> type))
	{
		std::cerr << "Error in packet : Invalid packet type. Disconnecting." << std::endl;
		return false;
	}

	//Each packet type is handled differently
	bool success = false;
	switch(type)
	{
		case (sf::Uint8)PacketType::NewPlayer:
			success = onNewPlayerPacket(packet);
			break;

		case (sf::Uint8)PacketType::Disconnection:
			success = onDisconnectionPacket(packet);
			break;

		case (sf::Uint8)PacketType::Map:
			success = onMapPacket(packet);
			break;

		case (sf::Uint8)PacketType::NewCharacter:
			success = onNewCharacterPacket(packet);
			break;

		case (sf::Uint8)PacketType::RemoveCharacters:
			success = onRemoveCharactersPacket(packet);
			break;

		case (sf::Uint8)PacketType::SetDirection:
			success = onSetDirectionPacket(packet);
			break;

		default:
			std::cerr << "Unknown packet type." << std::endl;
			break;
	}
	if(!success)
		std::cerr << "Error in packet : Invalid packet of type " << (int)type << ". Disconnecting." << std::endl;

	return success;
}

bool ClientSimulator::onNewPlayerPacket(sf::Packet &packet)
{
	//Get the newly connected player
	Player player;
	if(!(packet >> player))
		return false;
	//Add it to the game
	if(!addPlayer(std::move(player)))
		return false;
	return true;
}

bool ClientSimulator::onDisconnectionPacket(sf::Packet &packet)
{
	//Which player ? For what reason ?
	sf::Uint8 id;
	if(!(packet >> id))
		return false;
	if(id == NEUTRAL_PLAYER)
	{
		std::cerr << "Error : the neutral player cannot disconnect." << std::endl;
		return false;
	}
	//Remove it from the game
	if(!removePlayer(id))
		return false;
	return true;
}

bool ClientSimulator::onMapPacket(sf::Packet &packet)
{
	//Map name ?
	std::string name;
	if(!(packet >> name))
		return false;
	if(!loadMap(name))
		return false;
	return true;
}

bool ClientSimulator::onNewCharacterPacket(sf::Packet &packet)
{
	//Extract new player data
	Character character;
	if(!character.loadFromPacket(packet))
		return false;
	addCharacter(std::move(character));
	return true;
}

bool ClientSimulator::onRemoveCharactersPacket(sf::Packet &packet)
{
	//Treat all the characters until NO_CHARACTER_ID is received
	sf::Uint16 charid;
	while((packet >> charid))
	{
		if(charid == NO_CHARACTER_ID)
			return true;
		if(!removeCharacter(charid))
			return false;
	}
	//Error in packet, NO_CHARACTER_ID not reached
	return false;
}

bool ClientSimulator::onSetDirectionPacket(sf::Packet &packet)
{
	sf::Uint16 charid;
	sf::Vector2f direction;
	//Which character ? What direction ?
	if(!(packet >> charid >> direction.x >> direction.y))
		return false;
	try
	{
		getCharacter(charid).setDirection(direction);
	}
	catch(const std::out_of_range &)
	{
		//The character does not exist.
		std::cerr << "No character corresponds to id " << (int)charid << "." << std::endl;
		return false;
	}
	return true;
}

void ClientSimulator::netThread()
{
	sf::SocketSelector selector;
	m_server.addTo(selector);

	while(m_thrrunning)
	{
		//Wait until new new data or timeout
		if(selector.wait(SELECTOR_WAIT_TIME))
			m_thrrunning = receivePackets();
	}

	m_server.disconnect();
}

bool ClientSimulator::receivePackets()
{
	sf::Socket::Status status;
	sf::Packet *packet = new sf::Packet;
	//Receive all the packets
	while((status = m_server.receive(*packet)) == sf::Socket::Done)
	{
		m_receivedpackets.emplaceBack(packet);
		packet = new sf::Packet;
	}
	//Delete the last (unused) packet
	delete packet;
	//Error, or just no packets left ?
	if(status == sf::Socket::NotReady)
		return true;
	else if(status == sf::Socket::Error)
		std::cerr << "Unexpected network error." << std::endl;
	return false;
}
