#include "ClientSimulator.hpp"
#include "NetworkCodes.hpp"
#include "make_unique.hpp"
#include <iostream>

static const sf::Time SELECTOR_WAIT_TIME = sf::seconds(0.2f);
static const sf::Time CONNECTION_MAX_TIME = sf::seconds(7.f);
static const sf::Time CONNECTION_INFO_MAX_TIME = sf::seconds(10.f);

//TODO: Check each received value (e.g. Does the id exist ? Is it different of NO_ENTITY_ID ?)

ClientSimulator::ClientSimulator():
	GameSimulator(false, DEFAULT_INTERPOLATION_TIME),
	m_thread(nullptr),
	m_thrrunning(false),
	m_snapshotid(0),
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
	m_receivedpackets.treat([this, &success](std::unique_ptr<sf::Packet> &packet)
	{
		if(success)
			success = parseReceivedPacket(*packet);
	});

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
		m_thread = make_unique<std::thread>(&ClientSimulator::netThread, this);
	}
	catch(const std::exception &e)
	{
		m_thrrunning = false;
		std::cerr << "Could not start networking thread." << std::endl;
		std::cerr << e.what() << std::endl;
		m_thread.reset();
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
		m_thread.reset();

		//Delete the remaining packets
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

void ClientSimulator::selfCastSpell(const Spell &spell)
{
	sf::Packet packet;
	packet << (sf::Uint8)PacketType::CastSpell << (sf::Uint8)spell.state << (sf::Uint8)spell.id;
	//Add additionnal spell characteristics
	switch(spell.getAssociatedType())
	{
		case Spell::Type::None:
			return;

		case Spell::Type::LineSpell:
			packet << spell.targetpoint.x << spell.targetpoint.y;
	}
	m_server.send(packet);
}

bool ClientSimulator::onSnapshotReceived(sf::Packet &packet)
{
	//Extract sequence number, and ignore the snapshot if the sequence number does not match
	sf::Uint32 snapshotseq;
	if(!(packet >> snapshotseq))
	{
		std::cerr << "Error in snapshot packet." << std::endl;
		return false;
	}
	if(snapshotseq != m_seqnumber)
	{
#ifndef NDEBUG
		std::cout << "[DEBUG]Snapshot ignored because of different sequence number : received " << snapshotseq << ", but expected " << m_seqnumber << '.' << std::endl;
#endif
		return true;
	}
	//Extract all entities info
	sf::Uint16 entid;
	float x, y;
	sf::Vector2f direction;
	std::list<sf::Uint16> visibleents;
	//Extract id
	if(!(packet >> entid))
	{
		std::cerr << "Error in snapshot packet." << std::endl;
		return false;
	}
	//All entities info until NO_ENTITY_ID
	while(entid != NO_ENTITY_ID)
	{
		//Extract info
		if(!(packet >> x >> y))
		{
			std::cerr << "Error in snapshot packet." << std::endl;
			return false;
		}
		GameEntity *entity = getEntity(entid);
		if(entity)
		{
			//Apply general position modifications
			//If the entity was not visible during last snapshot, force its position
			if(entity->getLastSnapshotId() != m_snapshotid - 1 && entity->getLastSnapshotId() != m_snapshotid)
				entity->forcePosition(x, y);
			else
				entity->setPosition(x, y);
			//Apply modifications depending on the entity type
			if(Character *character = dynamic_cast<Character *>(entity))
			{
				if(!(packet >> direction.x >> direction.y))
				{
					std::cerr << "Error in snapshot packet." << std::endl;
					return false;
				}
				character->setDirection(direction);
			}
			//Add it to the visible entities list
			visibleents.emplace_back(entid);
			entity->setLastSnapshotId(m_snapshotid);
		}
		//Extract next id
		if(!(packet >> entid))
		{
			std::cerr << "Error in snapshot packet." << std::endl;
			return false;
		}
	}
	if(m_statelistener)
		m_statelistener->onVisibleEntitiesChanged(std::move(visibleents));
	m_snapshotid++;
	return true;
}

bool ClientSimulator::parseConnectionData(sf::Packet &packet)
{
	sf::Uint8 playerscount;
	if(!(packet >> m_seqnumber >> playerscount >> m_ownid))
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
	//Get the entities
	sf::Uint8 entitytype;
	if(!(packet >> entitytype))
		return false;
	while(entitytype != (sf::Uint8)EntityType::None)
	{
		if(!addNetworkEntity(entitytype, packet))
			return false;
		if(!(packet >> entitytype))
			return false;
	}
	//Get the associated character
	sf::Uint16 charid;
	if(!(packet >> charid))
		return false;
	if(!setOwnCharacter(charid))
		return false;
	return true;
}

bool ClientSimulator::parseReceivedPacket(sf::Packet &packet)
{
	m_seqnumber++;
	sf::Uint8 type;
	while((packet >> type))
	{
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

			case (sf::Uint8)PacketType::NewEntity:
				success = onNewEntityPacket(packet);
				break;

			case (sf::Uint8)PacketType::RemoveEntity:
				success = onRemoveEntityPacket(packet);
				break;

			default:
				std::cerr << "Unknown packet type." << std::endl;
				break;
		}
		if(!success)
		{
			std::cerr << "Error in packet : Invalid packet of type " << (int)type << ". Disconnecting." << std::endl;
			return false;
		}
	}

	return true;
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
	sf::Uint8 reason;
	if(!(packet >> id >> reason))
		return false;
	if(id == NEUTRAL_PLAYER)
	{
		std::cerr << "Error : the neutral player cannot disconnect." << std::endl;
		return false;
	}
	//Remove it from the game
	if(!removePlayer(id, reason))
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

bool ClientSimulator::onNewEntityPacket(sf::Packet &packet)
{
	//Extract new entity data
	sf::Uint8 enttype;
	if(!(packet >> enttype))
		return false;
	return addNetworkEntity(enttype, packet);
}

bool ClientSimulator::onRemoveEntityPacket(sf::Packet &packet)
{
	//Remove the associated entity
	sf::Uint16 entid;
	if(!(packet >> entid))
		return false;
	if(entid == NO_ENTITY_ID)
		return false;
	//Even if we cannot remove the entity, return true (the server might have failed to notify the creation of the entity in the first place
	if(!removeEntity(entid))
	{
#ifndef NDEBUG
		std::cerr << "[DEBUG]Client was told to remove entity " << (int)entid << " but it failed." << std::endl;
#endif
	}
	return true;
}

void ClientSimulator::netThread()
{
	sf::SocketSelector selector;
	m_server.addTo(selector);

	while(m_thrrunning)
	{
		//Wait until new data or timeout
		if(selector.wait(SELECTOR_WAIT_TIME))
			m_thrrunning = receivePackets();
	}

	m_server.disconnect();
}

bool ClientSimulator::receivePackets()
{
	sf::Socket::Status status;
	auto packet = make_unique<sf::Packet>();
	//Receive all the packets
	while((status = m_server.receive(*packet)) == sf::Socket::Done)
	{
		m_receivedpackets.emplaceBack(std::move(packet));
		packet = make_unique<sf::Packet>();
	}
	//Error, or just no packets left ?
	if(status == sf::Socket::NotReady)
		return true;
	else if(status == sf::Socket::Error)
		std::cerr << "Unexpected network error." << std::endl;
	return false;
}
