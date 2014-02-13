#include "ServerSimulator.hpp"
#include <iostream>
#include <sstream>
#include "NetworkCodes.hpp"

static const sf::Time SELECTOR_WAIT_TIME = sf::seconds(0.2f);

ServerSimulator::ServerSimulator():
	m_thread(nullptr),
	m_thrrunning(false),
	m_playerscount(0)
{
	m_playersids.reserveID(NEUTRAL_PLAYER);
}

ServerSimulator::~ServerSimulator()
{
	stopNetThread();
	for(std::pair<sf::TcpSocket *, Player> &newplayers : m_acceptedplayers)
		delete newplayers.first;
}

void ServerSimulator::update(float etime)
{
	//Examine network info
	//First, connections
	{//Lock m_acceptmutex & m_playerslistmutex
		std::lock_guard<std::mutex> lock(m_acceptmutex);
		std::lock_guard<std::mutex> lock2(m_playerslistmutex);
		for(std::pair<sf::TcpSocket *, Player> &accepted : m_acceptedplayers)
			addPlayer(std::move(accepted.second));
		m_acceptedplayers.clear();
	}
	//Then packets
	{//Lock m_receivemutex
		std::lock_guard<std::mutex> lock(m_receivemutex);
		for(std::pair<sf::Uint8, sf::Packet> &packet : m_receivedpackets)
		{

		}
		m_receivedpackets.clear();
	}
	//Then disconnections
	{//Lock m_disconnectmutex & m_playerslistmutex
		std::lock_guard<std::mutex> lock(m_disconnectmutex);
		std::lock_guard<std::mutex> lock2(m_playerslistmutex);
		for(sf::Uint8 id : m_disconnectedplayers)
		{
			removePlayer(id);
			m_playerscount--;
			m_clientstoremove.emplace_back(id);
		}
		m_disconnectedplayers.clear();
	}
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
		m_thread = nullptr;
	}
}

void ServerSimulator::netThread()
{
	sf::SocketSelector selector;
	selector.add(m_listener);

	std::list<sf::TcpSocket *> newclients;

	while(m_thrrunning)
	{
		//Remove clients to be removed
		{//Lock m_remclientmutex
			std::lock_guard<std::mutex> lock(m_remclientmutex);
			for(sf::Uint8 torem : m_clientstoremove)
			{
				m_clients[torem].removeFrom(selector);
				m_clients.erase(torem);
				m_playersids.releaseID(torem);
			}
			m_clientstoremove.clear();
		}
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
					if(it->second.isReady(selector))
					{
						if(!receiveNewPackets(it->first, it->second))
						{
							//Error or disconnection, kick the client
							it->second.removeFrom(selector);
							it->second.disconnect();
							{//Lock m_disconnectmutex
								std::lock_guard<std::mutex> lock(m_disconnectmutex);
								m_disconnectedplayers.push_back(it->first);
							}
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

	//Delete all the new clients that haven't been accepted yet
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
			return false; //No data to be received, don't do anything
		//On error, remove the new client
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
		//On error, remove the new client
		std::cerr << "Invalid connection data received from client." << std::endl;
		selector.remove(*socket);
		socket->disconnect();
		delete socket;
		return true;
	}
	//Find a new id if the game isn't full
	sf::Uint8 id;
	if(m_playerscount < m_maxplayers)
		id = player.id = m_playersids.getNewID();
	else
	{
		//Game is full, tell it to the client & remove it
		std::cerr << "Game is full. Disconnecting the new client." << std::endl;
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
	{//Lock m_acceptmutex
		std::lock_guard<std::mutex> lock(m_acceptmutex);
		//Add the player
		m_acceptedplayers.emplace_back(std::make_pair(socket, std::move(player)));
		m_clients.emplace(id, socket);
		m_playerscount++;

		//Put the current state (list of players) into a packet
		packet.clear();
		{//Lock m_playerslistmutex
			std::lock_guard<std::mutex> lock(m_playerslistmutex);
			const std::unordered_map<sf::Uint8, Player> &players = getPlayers();
			packet << (sf::Uint8)ConnectionStatus::Accepted << (sf::Uint8)m_playerscount << id;
			//List of players = all players + newly (totally) accepted players
			for(auto it = players.cbegin(); it != players.cend(); it++)
				packet << it->second;
		}
		for(const std::pair<sf::TcpSocket *, Player> &toadd : m_acceptedplayers)
			packet << toadd.second;
		if((status = socket->send(packet)) != sf::Socket::Done)
		{
			if(status == sf::Socket::Disconnected)
				std::cerr << "A new client disconnected before connection data could be sent." << std::endl;
			else if(status == sf::Socket::Error)
				std::cerr << "Unexpected error while sending connection data to a new client." << std::endl;
			else if(status == sf::Socket::NotReady)
				std::cerr << "Cannot accept client : socket buffer is full." << std::endl;//Impossible, right ?
			//On error, remove the new client
			m_acceptedplayers.pop_back();
			m_clients.erase(id);
			m_playerscount--;
			selector.remove(*socket);
			socket->disconnect();
			delete socket;
			return true;
		}
	}
	return true;
}

bool ServerSimulator::receiveNewPackets(sf::Uint8 id, SafeSocket &socket)
{
	sf::Socket::Status status;
	//Receive the packets
	{//Lock m_receivemutex
		std::lock_guard<std::mutex> lock(m_receivemutex);
		do
		{
			m_receivedpackets.emplace_back();
			m_receivedpackets.back().first = id;
		}
		while((status = socket.receive(m_receivedpackets.back().second)) == sf::Socket::Done);
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
