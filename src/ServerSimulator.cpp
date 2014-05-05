#include "ServerSimulator.hpp"
#include <iostream>
#include <sstream>
#include "NetworkCodes.hpp"
#include "Config.hpp"
#include <cassert>
#include "ServerUdpManager.hpp"

//TODO: Check each received value (e.g. Does the id exist ? Is it different from NO_CHARACTER_ID ?)

static const sf::Time SELECTOR_WAIT_TIME = sf::seconds(0.2f);

ServerSimulator::ServerSimulator(bool pure):
	GameSimulator(true, 0.f),
	m_thread(nullptr),
	m_thrrunning(false),
	m_udpmgr(*this)
{
	m_playersids.reserveID(NEUTRAL_PLAYER);
	m_charactersids.reserveID(NO_CHARACTER_ID);
	//If it's not a pure server, add a player (this player)
	if(!pure)
	{
		m_ownid = m_playersids.getNewID();
		Character *self = addCharacter(m_charactersids.getNewID(), true, 0.f, m_ownid);
		m_playerschars[addPlayer(m_ownid, Config::getInstance().name)->id] = self;
		setOwnCharacter(self->getId());
	}
}

ServerSimulator::~ServerSimulator()
{
	stopNetThread();
}

bool ServerSimulator::update(float etime)
{
	using namespace std::placeholders;
	m_udpmgr.update(etime);
	//Examine network info
	//First, new connections
	m_acceptedplayers.foreach([this](std::tuple<sf::IpAddress, unsigned short, Player> &newplayer)
	{
		acceptNewPlayer(std::get<0>(newplayer), std::get<1>(newplayer), std::get<2>(newplayer));
	});
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

	bool rc = GameSimulator::update(etime);
	updateVisibility();
	return rc;
}

void ServerSimulator::buildSnapshotPacket(sf::Packet &packet, sf::Uint8 playerid)
{
	packet << (sf::Uint8)UdpPacketType::Snapshot;
	Character *viewer = m_playerschars.at(playerid);
	if(viewer)
	{
		//Get visible characters & add them to the packet
		std::list<CollisionObject *> visible;
		getObjectsVisibleFrom(viewer, visible);
		for(CollisionObject *object : visible)
		{
			if(object->getEntityType() == CollisionEntityType::Character)
			{
				const Character *character = static_cast<const Character *>(object->getEntity());
				packet << character->getId()
				       << (float)character->getPosition().x << (float)character->getPosition().y
				       << (float)character->getDirection().x << (float)character->getDirection().y;
			}
		}
		packet << NO_CHARACTER_ID;//End of packet
	}
	else
		packet << NO_CHARACTER_ID;//End of packet (no content)
}

bool ServerSimulator::loadMap(const std::string &mapname)
{
	//Try to load the map
	if(!GameSimulator::loadMap(mapname))
		return false;
	//Tell the players the map has changed
	sf::Packet packet;
	packet << (sf::Uint8)PacketType::Map << mapname;
	sendToAllPlayers(packet);
	return true;
}

bool ServerSimulator::startNetThread(unsigned short tcpport, unsigned short udpport, sf::Uint8 maxplayers)
{
	//If the thread already exists, don't start a new one
	if(m_thread)
		return false;

	//If no map is loaded, we won't be able to accept clients
	if(!getMap())
	{
		std::cerr << "Cannot start server : no loaded map." << std::endl;
		return false;
	}

	m_maxplayers = maxplayers;

	//Start listening and accepting connections, and set the listener to non-blocking mode
	if(m_listener.listen(tcpport) != sf::Socket::Done)
	{
		m_listener.close();
		std::cerr << "Cannot listen to port " << (int)tcpport << std::endl;
		return false;
	}
	m_listener.setBlocking(false);

	if(!m_udpmgr.startNetThread(udpport))
	{
		m_listener.close();
		return false;
	}

	//Create the thread
	try
	{
		m_thrrunning = true;
		m_thread = new std::thread(&ServerSimulator::netThread, this);
	}
	catch(const std::exception &e)
	{
		m_thrrunning = false;
		m_thread = nullptr;
		std::cerr << "Could not start networking thread." << std::endl;
		std::cerr << e.what() << std::endl;
		m_listener.close();
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

		//Stop the UDP thread
		m_udpmgr.stopNetThread();

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
		m_receivedpackets.clear();
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
			for(std::pair<const sf::Uint8, SafeSocket<sf::TcpSocket>> &client : m_clients)
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
	sf::Uint16 port;
	if(!(packet >> player.name >> port))
	{
		//On error, remove the new client
		std::cerr << "Invalid connection data received from client." << std::endl;
		selector.remove(*socket);
		socket->disconnect();
		delete socket;
		return true;
	}
	if(port == 0)
	{
		std::cerr << "Invalid connection data received from client (UDP port cannot be 0)." << std::endl;
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
	m_acceptedplayers.emplaceBack(socket->getRemoteAddress(), port, std::move(player));
	return true;
}

int ServerSimulator::receiveNewPackets(sf::Uint8 id, SafeSocket<sf::TcpSocket> &socket)
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

void ServerSimulator::acceptNewPlayer(const sf::IpAddress &address, unsigned short port, Player &toaccept)
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
	if(!m_udpmgr.addPlayer(toaccept.id, address, port))
	{
		//Address & port already used, tell it to the client & remove it
		std::cerr << "Address and port of the new client seem to be already used. Disconnecting it." << std::endl;
		packet.clear();
		packet << (sf::Uint8)ConnectionStatus::WrongAddress;
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
	//Map name
	packet << getMap().getName();
	//Add all characters
	const std::unordered_map<sf::Uint16, Character> &characters = getCharacters();
	packet << (sf::Uint16)(characters.size() + 1);
	for(const std::pair<const sf::Uint16, Character> &character : characters)
		character.second.writeToPacket(packet, true);
	//Also add a character for this player (and tell the client this is his character)
	Character newcharacter(m_charactersids.getNewID(), true, 0.f, toaccept.id);
	newcharacter.writeToPacket(packet, false);
	packet << (sf::Uint16)newcharacter.getId();
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
		m_udpmgr.removePlayer(toaccept.id);
		//Release the id of the character
		m_charactersids.releaseID(newcharacter.getId());
		return;
	}
	//Tell all the other players about the new character
	packet.clear();
	packet << (sf::Uint8)PacketType::NewCharacter;
	newcharacter.writeToPacket(packet, true);
	sendToAllPlayers(packet);
	//Tell all the other players a new player connected
	packet.clear();
	packet << (sf::Uint8)PacketType::NewPlayer << toaccept;
	sendToAllPlayers(packet);
	//Add the player & the character to the simulation
	assert(!playerExists(toaccept.id));
	m_playerschars[addPlayer(std::move(toaccept))->id] = addCharacter(std::move(newcharacter));
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

	bool success = false;
	switch(type)
	{
		case (sf::Uint8)PacketType::SetDirection:
			success = onSetDirectionPacketReceived(std::get<0>(received), *std::get<1>(received));
			break;

		default:
			std::cerr << "Invalid packet type." << std::endl;
			break;
	}
	if(!success)
	{
		std::cerr << "Invalid packet data. Disconnecting client." << std::endl;
		disconnectPlayer(std::get<0>(received), (sf::Uint8)DisconnectionReason::Error);
		return;
	}
}

void ServerSimulator::disconnectPlayer(sf::Uint8 id, sf::Uint8 reason)
{
	//Remove the player. Note : he may not exists if the client disconnects before being accepted or removed (e.g. the game is full).
	if(removePlayer(id, reason))
	{
		m_udpmgr.removePlayer(id);
		sf::Packet packet;
		//Remove the characters that need to be removed
		packet << (sf::Uint8)PacketType::RemoveCharacters;
		bool removed = false;
		const std::unordered_map<sf::Uint16, Character> &characters = getCharacters();
		auto it = characters.begin();
		while(it != characters.end())
		{
			if(it->second.getOwner() == id)
			{
				//Save the id and increment the iterator (so that the iterator isn't invalid after removing the character)
				sf::Uint16 remid = it->first;
				it++;
				removeCharacter(remid);
				//Add the removed character id to the packet
				packet << remid;
				removed = true;
			}
			else
				it++;
		}
		if(removed)
		{
			//Tell all the other players about the removed characters
			packet << (sf::Uint16)NO_CHARACTER_ID;
			sendToAllPlayers(packet);
		}
		//Tell all the other players about the disconnection
		packet.clear();
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

bool ServerSimulator::onSetDirectionPacketReceived(sf::Uint8 sender, sf::Packet &packet)
{
	sf::Vector2f direction;
	packet >> direction.x >> direction.y;
	try
	{
		m_playerschars.at(sender)->setDirection(direction);
	}
	catch(const std::out_of_range &)
	{
		//No character for player
		std::cerr << "No character associated to player " << (int)sender << ". Discarding packet." << std::endl;
		return true;//It is not because of the packet, do not disconnect the client
	}
	return true;
}

bool ServerSimulator::removeCharacter(sf::Uint16 id)
{
	if(GameSimulator::removeCharacter(id))
	{
		//If the character is played by a player, also remove the reference to it
		auto it = m_playerschars.begin();
		while(it != m_playerschars.end())
		{
			if(it->second->getId() == id)
				it = m_playerschars.erase(it);
			else
				it++;
		}
		m_charactersids.releaseID(id);
		return true;
	}
	return false;
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

void ServerSimulator::updateVisibility()
{
	if(!(getOwnCharacter() && m_statelistener))
		return;
	std::list<CollisionObject *> visible;
	std::list<sf::Uint16> characters;
	//Get visible objects
	getObjectsVisibleFrom(getOwnCharacter(), visible);
	//Add the id of each object into a list
	for(CollisionObject *object : visible)
	{
		if(object->getEntityType() == CollisionEntityType::Character)
			characters.emplace_back(static_cast<Character *>(object->getEntity())->getId());
	}
	//Send the list to the state listener
	m_statelistener->onVisibleEntitiesChanged(std::move(characters));
}
