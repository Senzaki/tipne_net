#include "ClientSimulator.hpp"
#include "NetworkCodes.hpp"
#include <iostream>

static const sf::Time SELECTOR_WAIT_TIME = sf::seconds(0.2f);
static const sf::Time CONNECTION_MAX_TIME = sf::seconds(15.f);
static const sf::Time CONNECTION_INFO_MAX_TIME = sf::seconds(10.f);

ClientSimulator::ClientSimulator():
	m_thread(nullptr),
	m_thrrunning(false)
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
		if(!success)
		{
			delete packet;
			return;//Since it's not really a loop (it's a lambda closure), return means "continue"
		}
		sf::Uint8 type;
		if(!(*packet >> type))
		{
			std::cerr << "Error in packet : Invalid packet type. Disconnecting." << std::endl;
			success = false;
		}
		else
		{
			switch(type)
			{
				case (sf::Uint8)PacketType::NewPlayer:
					success = onNewPlayerPacket(*packet);
					break;

				case (sf::Uint8)PacketType::Disconnection:
					success = onDisconnectionPacket(*packet);
					break;

				case (sf::Uint8)PacketType::Map:
					success = onMapPacket(*packet);
					break;
			}
			if(!success)
				std::cerr << "Error in packet : Invalid packet of type " << (int)type << ". Disconnecting." << std::endl;
		}
		delete packet;
	});
	m_receivedpackets.clear();

	if(!success)
		return false;
	return GameSimulator::update(etime);
}

int ClientSimulator::startNetThread(const sf::IpAddress &serveraddr, unsigned short port, const std::string &name)
{
	//If the thread already exists, don't start a new one
	if(m_thread)
		return -1;

	sf::Socket::Status status;
	//Connect to the server
	m_server.setBlocking(true);
	if((status = m_server.connect(serveraddr, port, CONNECTION_MAX_TIME)) != sf::Socket::Done)
		return -1;
	//Send connection info
	sf::Packet packet;
	packet << name;
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
	}
}

bool ClientSimulator::isConnected() const
{
	return m_thrrunning;
}

bool ClientSimulator::parseConnectionData(sf::Packet &packet)
{
	sf::Uint8 players;
	if(!(packet >> players >> m_ownid))
		return false;

	//Add all the players
	Player player;
	for(sf::Uint8 i = 0; i < players; i++)
	{
		if(!(packet >> player))
			return false;
		if(!addPlayer(std::move(player)))
			return false;
	}
	//Get the map
	sf::Uint8 mapid;
	if(!(packet >> mapid))
		return false;
	if(!m_map.load(mapid))
		return false;

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
	//Map id ?
	sf::Uint8 id;
	if(!(packet >> id))
		return false;
	if(!m_map.load(id))
		return false;
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
