#include "ServerSimulator.hpp"
#include <iostream>
#include <sstream>
#include "NetworkCodes.hpp"
#include "Config.hpp"
#include <cassert>

static const sf::Time SELECTOR_WAIT_TIME = sf::seconds(0.2f);

ServerSimulator::ServerSimulator(bool pure):
	m_thread(nullptr),
	m_thrrunning(false)
{
	m_playersids.reserveID(NEUTRAL_PLAYER);
	//If it's not a pure server, add a player (this player)
	if(!pure)
	{
		m_ownid = m_playersids.getNewID();
		addPlayer(m_ownid, Config::getInstance().name);
	}
}

ServerSimulator::~ServerSimulator()
{
	stopNetThread();
}

bool ServerSimulator::update(float etime)
{
	using namespace std::placeholders;
	//Examine network info
	//First, new connections
	m_acceptedplayers.foreach(std::bind(&ServerSimulator::acceptNewPlayer, this, _1));
	m_acceptedplayers.clear();
	//Then packets
	m_receivedpackets.foreach([this](std::tuple<sf::Uint8, sf::Packet *> &received)
	{
		parseNewPacket(received);
		delete std::get<1>(received);
	});
	m_receivedpackets.clear();
	//Then disconnections
	m_disconnectedplayers.foreach([this](std::tuple<sf::Uint8, sf::Uint8> &discoinfo)
	{
		disconnectPlayer(std::get<0>(discoinfo), std::get<1>(discoinfo));
	});
	m_disconnectedplayers.clear();

	return GameSimulator::update(etime);
}

bool ServerSimulator::loadMap(sf::Uint8 mapid)
{
	//Try to load the map
	if(!m_map.load(mapid))
		return false;
	//Tell the players the map has changed
	sf::Packet packet;
	packet << (sf::Uint8)PacketType::Map << mapid;
	sendToAllPlayers(packet);
	return true;
}

bool ServerSimulator::startNetThread(unsigned short port, sf::Uint8 maxplayers)
{
	//If the thread already exists, don't start a new one
	if(m_thread)
		return false;

	//If no map is loaded, we won't be able to accept clients
	if(!m_map)
	{
		std::cerr << "Cannot start server : no loaded map." << std::endl;
		return false;
	}

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
		//Stop it
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
		m_receivedpackets.foreach([](std::tuple<sf::Uint8, sf::Packet *> &received)
		{
			delete std::get<1>(received);
		});
	}
}

void ServerSimulator::netThread()
{
	sf::SocketSelector selector;
	selector.add(m_listener);

	std::list<sf::TcpSocket *> newclients;

	while(m_thrrunning)
	{
		{//Lock m_clientsmutex
			std::lock_guard<std::mutex> lock(m_clientsmutex);
			//Remove clients to be removed
			m_clientstoremove.foreach([this, &selector](sf::Uint8 torem)
			{
				m_clients[torem].removeFrom(selector);
				m_clients.erase(torem);
				m_playersids.releaseID(torem);
			});
			m_clientstoremove.clear();
		}

		//Wait until new connection, new data or timeout
		if(selector.wait(SELECTOR_WAIT_TIME))
		{
			//New connection ?
			if(selector.isReady(m_listener))
				acceptNewConnections(newclients, selector);
			//Client ?
			for(std::pair<const sf::Uint8, SafeSocket> &client : m_clients)
			{
				if(client.second.isReady(selector))
				{
					int discoreason;
					if((discoreason = receiveNewPackets(client.first, client.second)) >= 0)
					{
						//Error or disconnection, kick the client
						client.second.removeFrom(selector);
						client.second.disconnect();
						m_disconnectedplayers.pushBack(std::make_pair(client.first, (sf::Uint8)discoreason));
					}
				}
			}
			//New client ?
			{
				auto it = newclients.begin();
				while(it != newclients.end())
				{
					if(selector.isReady(**it))
					{
						//Remove it from the new client if info was received (or error)
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
	//Get a new ID if possible
	try
	{
		player.id = m_playersids.getNewID();
	}
	catch(const std::length_error &)
	{
		//Game is full, tell it to the client & remove it
		std::cerr << "Game is full. Disconnecting the new client." << std::endl;
		packet.clear();
		packet << (sf::Uint8)ConnectionStatus::GameIsFull;
		socket->send(packet);
		selector.remove(*socket);
		delete socket;
		return true;
	}
	//Add it to the playing clients list
	{//Lock m_clientsmutex
		std::lock_guard<std::mutex> lock(m_clientsmutex);
		m_clients.emplace(player.id, socket);
	}
	//Send the new player to the main thread
	m_acceptedplayers.emplaceBack(std::move(player));
	return true;
}

int ServerSimulator::receiveNewPackets(sf::Uint8 id, SafeSocket &socket)
{
	sf::Socket::Status status;
	sf::Packet *packet = new sf::Packet;
	//Receive all the packets
	while((status = socket.receive(*packet)) == sf::Socket::Done)
	{
		m_receivedpackets.emplaceBack(id, packet);
		packet = new sf::Packet;
	}
	//Delete the last (unused) packet
	delete packet;
	//Error, or just no packets left ?
	if(status == sf::Socket::NotReady)
		return -1;
	else if(status == sf::Socket::Error)
	{
		std::cerr << "Unexpected network error." << std::endl;
		return (int)DisconnectionReason::Error;
	}
	return (int)DisconnectionReason::Left;
}

void ServerSimulator::acceptNewPlayer(Player &toaccept)
{
	sf::Packet packet;
	//If the room is full, disconnect the player
	if(getPlayers().size() == m_maxplayers)
	{
		//Game is full, tell it to the client & remove it
		std::cerr << "Game is full. Disconnecting the new client." << std::endl;
		packet.clear();
		packet << (sf::Uint8)ConnectionStatus::GameIsFull;
		sendToPlayer(toaccept.id, packet);
		m_clientstoremove.emplaceBack(toaccept.id);
		return;
	}
	//Add a number after the name if someone already has this name (not very optimized :p)
	int suffixnbr = 1;
	std::string newname = toaccept.name;
	//Increment the number while the name exists
	while(playerNameExists(newname))
	{
		suffixnbr++;
		std::ostringstream strm(toaccept.name + " (", std::ostringstream::ate);
		strm << suffixnbr << ')';
		newname = strm.str();
	}
	toaccept.name = std::move(newname);

	//Put the current state into a packet
	packet.clear();
	const std::unordered_map<sf::Uint8, Player> &players = getPlayers();
	packet << (sf::Uint8)ConnectionStatus::Accepted << (sf::Uint8)(players.size() + 1) << toaccept.id;
	//List of players = all players + new player
	for(const std::pair<const sf::Uint8, Player> &player : players)
		packet << player.second;
	packet << toaccept;
	//Map Id
	packet << (sf::Uint8)m_map.getID();
	//Try to send it
	sf::Socket::Status status;
	if((status = sendToPlayer(toaccept.id, packet)) != sf::Socket::Done)
	{
		if(status == sf::Socket::Disconnected)
			std::cerr << "A new client disconnected before connection data could be sent." << std::endl;
		else if(status == sf::Socket::Error)
			std::cerr << "Unexpected error while sending connection data to a new client." << std::endl;
		else if(status == sf::Socket::NotReady)
			std::cerr << "Cannot accept client : socket buffer is full." << std::endl;//Impossible, right ?
		//On error, remove the new client
		m_clientstoremove.emplaceBack(toaccept.id);
		return;
	}
	packet.clear();
	//Tell all the other players a new player connected
	packet << (sf::Uint8)PacketType::NewPlayer << toaccept;
	sendToAllPlayers(packet);
	//Add the player to the simulation
	assert(!playerExists(toaccept.id));
	addPlayer(std::move(toaccept));
}

void ServerSimulator::parseNewPacket(std::tuple<sf::Uint8, sf::Packet *> &received)
{
	if(!playerExists(std::get<0>(received)))
	{
#ifndef NDEBUG
		std::cerr << "[DEBUG]Packet received from unknown client. Discarding it." << std::endl;
#endif
		return;
	}
	sf::Uint8 type;
	sf::Packet &packet = *std::get<1>(received);
	if(!(packet >> type))
	{
		std::cerr << "Invalid packet received. Disconnecting client." << std::endl;
		disconnectPlayer(std::get<0>(received), (sf::Uint8)DisconnectionReason::Error);
		return;
	}
	switch(type)
	{
		default:
			std::cerr << "Invalid packet type. Disconnecting client." << std::endl;
			disconnectPlayer(std::get<0>(received), (sf::Uint8)DisconnectionReason::Error);
			return;
	}
}

void ServerSimulator::disconnectPlayer(sf::Uint8 id, sf::Uint8 reason)
{
	//Remove the player. Note : he may not exists if the client disconnects before being accepted or removed (e.g. the game is full).
	if(removePlayer(id, reason))
	{
		//Tell all the other players
		sf::Packet packet;
		packet << (sf::Uint8)PacketType::Disconnection << id << reason;
		sendToAllPlayers(packet);
	}
	//Tell the child thread to remove the client
	m_clientstoremove.emplaceBack(id);
}

sf::Socket::Status ServerSimulator::sendToPlayer(sf::Uint8 id, sf::Packet &packet)
{
	//Lock m_clientsmutex
	std::lock_guard<std::mutex> lock(m_clientsmutex);
	return m_clients[id].send(packet);
}

void ServerSimulator::sendToAllPlayers(sf::Packet &packet)
{
	const std::unordered_map<sf::Uint8, Player> &players = getPlayers();
	//Lock m_clientsmutex
	std::lock_guard<std::mutex> lock(m_clientsmutex);
	//Send to all
	for(const std::pair<const sf::Uint8, Player> &player : players)
		m_clients[player.first].send(packet);
}

bool ServerSimulator::playerNameExists(const std::string &name) const
{
	//Simply iterate through the whole player list to find the corresponding name
	for(auto it = getPlayers().cbegin(); it != getPlayers().cend(); it++)
	{
		if(it->second.name == name)
			return true;
	}
	return false;
}
