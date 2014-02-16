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

void ClientSimulator::update(float etime)
{
	//Treat packets
	{//Lock m_receivemutex
		std::lock_guard<std::mutex> lock(m_receivemutex);
		sf::Uint8 type;
		for(sf::Packet &packet : m_receivedpackets)
		{
			if(!(packet >> type))
				std::cerr << "Error in packet : Invalid packet type." << std::endl;
			else
			{
				bool success = false;
				switch(type)
				{
					case (sf::Uint8)PacketType::NewPlayer:
						success = onNewPlayerPacket(packet);
						break;

					case (sf::Uint8)PacketType::Disconnection:
						success = onDisconnectionPacket(packet);
						break;
				}
				if(!success)
					std::cerr << "Error in packet : Invalid packet of type " << (int)type << "." << std::endl;
			}
		}
		m_receivedpackets.clear();
	}
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
	}
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
		addPlayer(std::move(player));
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
	addPlayer(std::move(player));
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
	removePlayer(id);
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
		{
			m_thrrunning = receivePackets();
		}
	}

	m_server.disconnect();
}

bool ClientSimulator::receivePackets()
{
	sf::Socket::Status status;
	//Receive the packets
	{//Lock m_receivemutex
		std::lock_guard<std::mutex> lock(m_receivemutex);
		do
		{
			m_receivedpackets.emplace_back();
		} while((status = m_server.receive(m_receivedpackets.back())) == sf::Socket::Done);
		//Remove the last packet, because it was not used
		m_receivedpackets.pop_back();
	}
	//Error, or just no packets left ?
	if(status == sf::Socket::NotReady)
		return true;
	else if(status == sf::Socket::Error)
		std::cerr << "Unexpected network error." << std::endl;
	return false;
}
