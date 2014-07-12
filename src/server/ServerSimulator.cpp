#include "ServerSimulator.hpp"
#include <iostream>
#include <sstream>
#include "NetworkCodes.hpp"
#include "Config.hpp"
#include "Spell.hpp"
#include <cassert>
#include "ServerUdpManager.hpp"

//TODO: Check each received value (e.g. Does the id exist ? Is it different from NO_ENTITY_ID ?)

static const sf::Time SELECTOR_WAIT_TIME = sf::seconds(0.2f);

ServerSimulator::ServerSimulator(bool pure):
	GameSimulator(true, 0.f),
	m_thrrunning(false),
	m_udpmgr(*this),
	m_seqnumber(0)
{
	m_playersids.reserveID(NEUTRAL_PLAYER);
	resetRoundInfo();
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
	m_acceptedplayers.treat([this](std::tuple<sf::IpAddress, unsigned short, Player> &newplayer)
	{
		acceptNewPlayer(std::get<0>(newplayer), std::get<1>(newplayer), std::get<2>(newplayer));
	});
	//Then packets
	m_receivedpackets.treat([this](std::tuple<sf::Uint8, std::unique_ptr<sf::Packet>> &received)
	{
		parseNewPacket(received);
	});
	//Then disconnections
	m_disconnectedplayers.treat([this](std::tuple<sf::Uint8, sf::Uint8> &discoinfo)
	{
		disconnectPlayer(std::get<0>(discoinfo), std::get<1>(discoinfo));
	});

	bool rc = GameSimulator::update(etime);
	updateVisibility();
	sendGeneralPacket();
	m_udpmgr.update(etime);
	return rc;
}

void ServerSimulator::buildSnapshotPacket(sf::Packet &packet, sf::Uint8 playerid)
{
	packet << (sf::Uint8)UdpPacketType::Snapshot;
	packet << m_seqnumber;
	Character *viewer = m_playerschars.at(playerid);
	if(viewer)
	{
		//Get visible characters & add them to the packet
		std::list<CollisionObject *> visible;
		m_round->getObjectsVisibleFrom(viewer, visible);
		for(CollisionObject *object : visible)
		{
			if(object->getEntityType() == CollisionEntityType::Entity)
			{
				//Add general entity data
				const GameEntity *entity = static_cast<const GameEntity *>(object->getEntity());
				packet << entity->getId()
				       << (float)entity->getPosition().x << (float)entity->getPosition().y;
				//Add data specific to the entity type
				if(const Character *character = dynamic_cast<const Character *>(entity))
					packet << (float)character->getDirection().x << (float)character->getDirection().y;
			}
		}
	}
	packet << NO_ENTITY_ID;//End of packet
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
		m_thread = make_unique<std::thread>(&ServerSimulator::netThread, this);
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
		m_thread.reset();

		//Delete the remaining packets
		m_receivedpackets.clear();
	}
}

void ServerSimulator::netThread()
{
	sf::SocketSelector selector;
	selector.add(m_listener);

	std::list<std::unique_ptr<sf::TcpSocket>> newclients;

	while(m_thrrunning)
	{
		{//Lock m_clientsmutex
			std::lock_guard<std::mutex> lock(m_clientsmutex);
			//Remove clients to be removed
			m_clientstoremove.treat([this, &selector](sf::Uint8 torem)
			{
				m_clients[torem].removeFrom(selector);
				m_clients.erase(torem);
				m_playersids.releaseID(torem);
			});
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

	//Disconnect all the new clients that haven't been accepted yet
	for(std::unique_ptr<sf::TcpSocket> &socket : newclients)
		socket->disconnect();
}

void ServerSimulator::acceptNewConnections(std::list<std::unique_ptr<sf::TcpSocket>> &newclients, sf::SocketSelector &selector)
{
	std::list<std::unique_ptr<sf::TcpSocket>> accepted;
	sf::Socket::Status status;
	//A new client tries to connect
	do
	{
		accepted.emplace_back(make_unique<sf::TcpSocket>());
	} while((status = m_listener.accept(*accepted.back())) == sf::Socket::Done);

	//Remove the last client client from the list, because status != sf::Socket::Done
	accepted.pop_back();
	//Add the clients to the newclient list and to the selector
	if(!accepted.empty())
	{
		for(std::unique_ptr<sf::TcpSocket> &socket : accepted)
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

bool ServerSimulator::receivePlayerConnectionInfo(std::unique_ptr<sf::TcpSocket> &socket, sf::SocketSelector &selector)
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
		return true;
	}
	if(port == 0)
	{
		std::cerr << "Invalid connection data received from client (UDP port cannot be 0)." << std::endl;
		selector.remove(*socket);
		socket->disconnect();
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
		return true;
	}
	{//Lock m_clientsmutex
		//Send the new player to the main thread
		m_acceptedplayers.emplaceBack(socket->getRemoteAddress(), port, std::move(player));
		//Add it to the playing clients list
		std::lock_guard<std::mutex> lock(m_clientsmutex);
		m_clients.emplace(player.id, std::move(socket));
	}
	return true;
}

int ServerSimulator::receiveNewPackets(sf::Uint8 id, SafeSocket<sf::TcpSocket> &socket)
{
	sf::Socket::Status status;
	auto packet = make_unique<sf::Packet>();
	//Receive all the packets
	while((status = socket.receive(*packet)) == sf::Socket::Done)
	{
		m_receivedpackets.emplaceBack(id, std::move(packet));
		packet = make_unique<sf::Packet>();
	}
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

void ServerSimulator::selfCastSpell(const Spell &spell)
{
	Character *character = m_round->getOwnCharacter();
	if(!character)
	{
#ifndef NDEBUG
		std::cerr << "[DEBUG]selfCastSpell() function called, but no character exists for self." << std::endl;
#endif
		return;
	}
	if(!spell.castSpell(*m_round.get(), m_entitiesids, character))
	{
#ifndef NEDBUG
		std::cerr << "[DEBUG]selfCastSpell() function called, but spell casting error." << std::endl;
#endif
		return;
	}
}

void ServerSimulator::onNewRoundStarted(const std::string &mapname)
{
	resetRoundInfo();

	GameSimulator::onNewRoundStarted(mapname);

	//Tell the players a new round has started
	m_generalpacket << (sf::Uint8)PacketType::NewRound << mapname;

	//Add a character for each player
	const auto &players = getPlayers();
	for(const std::pair<const sf::Uint8, Player> &player : players)
	{
		sf::Uint8 playerid = player.first;
		Character *character = m_round->addEntity<Character>(*m_round.get(), m_entitiesids.getNewID());
		character->setOwner(playerid);
		m_playerschars[playerid] = character;
		if(playerid == m_ownid)
			m_round->setOwnCharacter(character);
	}
}

void ServerSimulator::onEntityAdded(GameEntity *entity)
{
	GameSimulator::onEntityAdded(entity);

	//Tell all the clients
	sf::Packet towrite;
	towrite << (sf::Uint8)PacketType::NewEntity;
	//If everything was successful, add it to the general packet
	if(RoundState::writeEntityInitData(entity, towrite, true))
		m_generalpacket.append(towrite.getData(), towrite.getDataSize());
}

void ServerSimulator::onEntityRemoved(GameEntity *entity)
{
	GameSimulator::onEntityRemoved(entity);

	//If the entity is a character played by a player, also remove the reference to it
	auto it = m_playerschars.begin();
	while(it != m_playerschars.end())
	{
		if(it->second->getId() == entity->getId())
			it = m_playerschars.erase(it);
		else
			it++;
	}
	m_entitiesids.releaseID(entity->getId());

	//Tell all the clients
	m_generalpacket << (sf::Uint8)PacketType::RemoveEntity << (sf::Uint16)entity->getId();
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

	assert(!playerExists(toaccept.id));
	//Tell all the other players a new player connected
	packet.clear();
	packet << (sf::Uint8)PacketType::NewPlayer << toaccept;
	sendToAllPlayers(packet);
	//Add the character to the simulation
	Character *newcharacter = m_round->addEntity<Character>(*m_round.get(), m_entitiesids.getNewID());
	//Send the general packet, the new client should not receive it
	sendGeneralPacket();
	//Add the player to the simulation
	Player *newplayer = addPlayer(std::move(toaccept));
	m_playerschars[newplayer->id] = newcharacter;
	newcharacter->setOwner(newplayer->id);

	//Put the current state into a packet
	packet.clear();
	const std::unordered_map<sf::Uint8, Player> &players = getPlayers();
	packet << (sf::Uint8)ConnectionStatus::Accepted << m_seqnumber << (sf::Uint8)players.size() << newplayer->id;
	//List of players
	for(const std::pair<const sf::Uint8, Player> &player : players)
		packet << player.second;
	//Map name
	packet << getMap().getName();
	//Add all entities
	const std::unordered_map<sf::Uint16, std::unique_ptr<GameEntity>> &entities = m_round->getEntities();
	for(const std::pair<const sf::Uint16, std::unique_ptr<GameEntity>> &entity : entities)
		RoundState::writeEntityInitData(entity.second.get(), packet, true);
	packet << (sf::Uint8)EntityType::None;
	//Tell the client this is his character
	packet << (sf::Uint16)newcharacter->getId();
	//Try to send it
	sf::Socket::Status status;
	if((status = sendToPlayer(newplayer->id, packet)) != sf::Socket::Done)
	{
		if(status == sf::Socket::Disconnected)
			std::cerr << "A new client disconnected before connection data could be sent." << std::endl;
		else if(status == sf::Socket::Error)
			std::cerr << "Unexpected error while sending connection data to a new client." << std::endl;
		else if(status == sf::Socket::NotReady)
			std::cerr << "Cannot accept client : socket buffer is full." << std::endl;//Impossible, right ?
		//On error, remove the new client
		disconnectPlayer(newplayer->id, (sf::Uint8)DisconnectionReason::Error);
		return;
	}
}

void ServerSimulator::parseNewPacket(std::tuple<sf::Uint8, std::unique_ptr<sf::Packet>> &received)
{
	if(!playerExists(std::get<0>(received)))
	{
#ifndef NDEBUG
		std::cerr << "[DEBUG]Packet received from unknown client. Discarding it." << std::endl;
#endif
		return;
	}
	//Get the type of the packet
	sf::Uint8 type;
	sf::Packet &packet = *std::get<1>(received);
	if(!(packet >> type))
	{
		std::cerr << "Invalid packet received. Disconnecting client." << std::endl;
		disconnectPlayer(std::get<0>(received), (sf::Uint8)DisconnectionReason::Error);
		return;
	}

	//Handle the packet depending on its type
	bool success = false;
	switch(type)
	{
		case (sf::Uint8)PacketType::SetDirection:
			success = onSetDirectionPacketReceived(std::get<0>(received), packet);
			break;

		case (sf::Uint8)PacketType::CastSpell:
			success = onCastSpellPacketReceived(std::get<0>(received), packet);
			break;

		default:
			std::cerr << "Invalid packet type." << std::endl;
			break;
	}
	//Not successful, disconnect the player...
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
		//Remove the entities that need to be removed
		const std::unordered_map<sf::Uint16, std::unique_ptr<GameEntity>> &entities = m_round->getEntities();
		auto it = entities.begin();
		while(it != entities.end())
		{
			if(it->second->getOwner() == id)
			{
				//Save the id and increment the iterator (so that the iterator isn't invalid after removing the entity)
				sf::Uint16 remid = it->first;
				it++;
				m_round->removeEntity(remid);
			}
			else
				it++;
		}
		//Tell all the other players about the disconnection
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
	m_seqnumber++;
	const std::unordered_map<sf::Uint8, Player> &players = getPlayers();
	//Lock m_clientsmutex
	std::lock_guard<std::mutex> lock(m_clientsmutex);
	//Send to all
	for(const std::pair<const sf::Uint8, Player> &player : players)
	{
		if(player.first != m_ownid)
			m_clients[player.first].send(packet);
	}
}

void ServerSimulator::sendGeneralPacket()
{
	//Send the messages to all clients
	if(m_generalpacket.getDataSize() != 0)
	{
		sendToAllPlayers(m_generalpacket);
		m_generalpacket.clear();
	}
}

bool ServerSimulator::onSetDirectionPacketReceived(sf::Uint8 sender, sf::Packet &packet)
{
	sf::Vector2f direction;
	if(!(packet >> direction.x >> direction.y))
		return false;
	try
	{
		m_playerschars.at(sender)->setDirection(direction);
	}
	catch(const std::out_of_range &)
	{
		//No character for player
#ifndef NDEBUG
		std::cerr << "[DEBUG]No character associated to player " << (int)sender << ". Discarding packet." << std::endl;
#endif
		return true;//It is not because of the packet, do not disconnect the client
	}
	return true;
}

bool ServerSimulator::onCastSpellPacketReceived(sf::Uint8 sender, sf::Packet &packet)
{
	sf::Uint8 state;
	sf::Uint8 id;
	if(!(packet >> state >> id))
		return false;
	Character *character;
	try
	{
		character = m_playerschars.at(sender);
	}
	catch(const std::out_of_range &)
	{
		//No character for player
#ifndef NDEBUG
		std::cerr << "[DEBUG]No character associated to player " << (int)sender << ". Discarding packet." << std::endl;
#endif
		return true;//It is not because of the packet, do not disconnect the client
	}
	if(state >= (sf::Uint8)Character::State::Count)
	{
#ifndef NDEBUG
		std::cerr << "[DEBUG]Invalid character state identifier for spell." << std::endl;
#endif
		return false;
	}
	Spell spell;
	spell.id = id;
	spell.state = static_cast<Character::State>(state);
	//Read additionnal spell characteristics
	switch(spell.getAssociatedType())
	{
		case Spell::Type::None:
#ifndef NDEBUG
			std::cerr << "[DEBUG]Spell does not exist." << std::endl;
#endif
			return false;

		case Spell::Type::LineSpell:
			if(!(packet >> spell.targetpoint.x >> spell.targetpoint.y))
				return false;
			break;
	}
	spell.castSpell(*m_round.get(), m_entitiesids, character);
	return true;
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

void ServerSimulator::resetRoundInfo()
{
	m_entitiesids.reserveID(NO_ENTITY_ID);
	m_playerschars.clear();
}

void ServerSimulator::updateVisibility()
{
	if(!(m_round->getOwnCharacter() && m_statelistener))
		return;
	std::list<CollisionObject *> visible;
	std::list<sf::Uint16> entities;
	//Get visible objects
	m_round->getObjectsVisibleFrom(m_round->getOwnCharacter(), visible);
	//Add the id of each object into a list
	for(CollisionObject *object : visible)
	{
		if(object->getEntityType() == CollisionEntityType::Entity)
			entities.emplace_back(static_cast<GameEntity *>(object->getEntity())->getId());
	}
	//Send the list to the state listener
	m_statelistener->onVisibleEntitiesChanged(std::move(entities));
}
