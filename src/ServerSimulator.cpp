#include "ServerSimulator.hpp"
#include <iostream>
#include <sstream>
#include "NetworkCodes.hpp"

static const sf::Time SELECTOR_WAIT_TIME = sf::seconds(0.2f);

ServerSimulator::ServerSimulator():
	m_thread(nullptr),
	m_thrrunning(false)
{
	m_playersids.reserveID(NEUTRAL_PLAYER);
}

ServerSimulator::~ServerSimulator()
{
	stopNetThread();
}

bool ServerSimulator::startNetThread(unsigned short port, sf::Uint8 maxplayers)
{
	//If the thread already exists, don't start a new one
	if(m_thread)
		return false;

	m_maxplayers = maxplayers;

	//Start listening and accepting connections, and set the listener to non-blocking mode
	if(m_listener.listen(port) != sf::Socket::Done)
	{
		m_listener.close();
		return false;
	}
	m_listener.setBlocking(false);

	//Create the thread
	try
	{
		m_thrrunning = true;
		m_thread = new std::thread(&ServerSimulator::netThread, this);
	}
	catch(const std::exception &e)
	{
		m_thrrunning = false;
		std::cerr << "Could not start networking thread." << std::endl;
		std::cerr << e.what() << std::endl;
		return false;
	}

	return true;
}

void ServerSimulator::stopNetThread()
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
	}
}

void ServerSimulator::netThread()
{
	sf::SocketSelector selector;
	selector.add(m_listener);

	std::list<sf::TcpSocket *> newclients;

	while(m_thrrunning)
	{
		//Wait until new connection, new data or timeout
		if(selector.wait(SELECTOR_WAIT_TIME))
		{
			//New connection ?
			if(selector.isReady(m_listener))
			{
				acceptNewConnections(newclients, selector);
			}
			//Client ?
			{
				auto it = m_clients.begin();
				while(it != m_clients.end())
				{
					if(selector.isReady(*it->second))
					{
						if(!receiveNewPacket(it->second))
						{//Lock net
							//Error or disconnection, kick the client
							std::lock_guard<std::mutex> netlock(m_netmutex);
							selector.remove(*it->second);
							it->second->disconnect();
							delete it->second;
							removePlayer(it->first);
							m_playersids.releaseID(it->first);
							it = m_clients.erase(it);
							continue;
						}
					}
					it++;
				}
			}
			//New client ?
			{
				auto it = newclients.begin();
				while(it != newclients.end())
				{
					if(selector.isReady(**it))
					{
						if(receivePlayerConnectionInfo(*it, selector))
						{
							it = newclients.erase(it);
							continue;
						}
					}
					it++;
				}
			}
		}
	}

	//Delete all the new clients
	for(sf::TcpSocket *socket : newclients)
	{
		socket->disconnect();
		delete socket;
	}
}

void ServerSimulator::acceptNewConnections(std::list<sf::TcpSocket *> &newclients, sf::SocketSelector &selector)
{
	std::list<sf::TcpSocket *> accepted;
	std::cout << "New connection !" << std::endl;
	sf::Socket::Status status;
	//A new client tries to connect
	do
	{
		accepted.emplace_back(new sf::TcpSocket);
	} while((status = m_listener.accept(*accepted.back())) == sf::Socket::Done);

	//Remove the last client client from the list, because status != sf::Socket::Done
	delete accepted.back();
	accepted.pop_back();
	//Add the clients to the newclient list and to the selector
	if(!accepted.empty())
	{
		for(sf::TcpSocket *socket : accepted)
		{
			selector.add(*socket);
			socket->setBlocking(false);
		}
		newclients.splice(newclients.end(), std::move(accepted));
	}
	//Error, or just sf::Socket::NotReady ?
	if(status == sf::Socket::Disconnected)
	{
		std::cerr << "The listener is no longer bound to the right port (for an unknown reason)." << std::endl;
		std::cerr << "No new client will be accepted." << std::endl;
	}
	else if(status == sf::Socket::Error)
		std::cerr << "Unexpected error while accepting a new client." << std::endl;
}

bool ServerSimulator::receivePlayerConnectionInfo(sf::TcpSocket *socket, sf::SocketSelector &selector)
{
	std::cout << "Player connection info !" << std::endl;
	sf::Socket::Status status;
	sf::Packet packet;

	//Receive info
	if((status = socket->receive(packet)) != sf::Socket::Done)
	{
		if(status == sf::Socket::Disconnected)
			std::cerr << "Client disconnected before sending connection information." << std::endl;
		else if(status == sf::Socket::Error)
			std::cerr << "An unexpected error happened while receiving connection information." << std::endl;
		else if(status == sf::Socket::NotReady)
			return false;
		selector.remove(*socket);
		socket->disconnect();
		delete socket;
		return true;
	}
	//Parse player info
	Player player;
	player.ai = false;
	if(!(packet >> player.name))
	{
		std::cerr << "Invalid connection data received from client." << std::endl;
		selector.remove(*socket);
		socket->disconnect();
		delete socket;
		return true;
	}
	//Find a new id if the game isn't full
	sf::Uint8 id;
	if(getPlayers().size() < m_maxplayers)
		id = player.id = m_playersids.getNewID();
	else
	{
		//Game is full, tell it to the client
		std::cerr << "Room is full. Disconnecting the new client." << std::endl;
		packet.clear();
		packet << (sf::Uint8)ConnectionStatus::GameIsFull;
		socket->send(packet);
		selector.remove(*socket);
		socket->disconnect();
		delete socket;
		return true;
	}
	//Add a number after the name if someone already has this name
	try
	{
		int &namecount = m_names.at(player.name);
		namecount++;
		std::ostringstream strm(player.name + '(');
		strm << namecount << ')';
		player.name = strm.str();
	}
	catch(const std::out_of_range &)
	{
		m_names[player.name] = 1;
	}
	//Add the player
	addPlayer(std::move(player));

	//Send the current state (list of players)
	packet.clear();
	auto &players = getPlayers();
	packet << (sf::Uint8)ConnectionStatus::Accepted << (sf::Uint8)players.size() << id;
	for(auto it = players.begin(); it != players.end(); it++)
	{
		packet << *it->second;
	}
	if((status = socket->send(packet)) != sf::Socket::Done)
	{
		if(status == sf::Socket::Disconnected)
			std::cerr << "A new client disconnected before connection data could be sent." << std::endl;
		else if(status == sf::Socket::Error)
			std::cerr << "Unexpected error while sending connection data to a new client." << std::endl;
		else if(status == sf::Socket::NotReady)
			std::cerr << "Cannot accept client : socket buffer is full." << std::endl;//Impossible, right ?
		removePlayer(id);
		selector.remove(*socket);
		socket->disconnect();
		delete socket;
		return true;
	}
	{//Lock net
		std::lock_guard<std::mutex> lock(m_netmutex);
		m_clients[id] = socket;
	}
	return true;
}

bool ServerSimulator::receiveNewPacket(sf::TcpSocket *socket)
{
	sf::Packet packet;
	sf::Socket::Status status;
	//Receive the packet
	if((status = socket->receive(packet)) != sf::Socket::Done)
	{
		if(status == sf::Socket::Error)
			std::cerr << "Unexpected network error." << std::endl;
		else if(status == sf::Socket::NotReady)
			return true;
		return false;
	}
	return true;
}

void ServerSimulator::sendPacket(sf::Uint8 dest, sf::Packet &packet)
{
	std::lock_guard<std::mutex> lock(m_netmutex);
	try
	{
		sf::TcpSocket *socket = m_clients.at(dest);
		socket->send(packet);
	}
	catch(const std::out_of_range &)
	{

	}
}
