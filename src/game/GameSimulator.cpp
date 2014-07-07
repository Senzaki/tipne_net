#include "GameSimulator.hpp"
#include <iostream>
#include <cassert>
#include <functional>
#include "DefaultCollisionManager.hpp"
#include "DummyCollisionManager.hpp"
#include "LineDamageSpell.hpp"

GameSimulator::GameSimulator(bool fullsimulator, float interpolationtime):
	m_ownid(NEUTRAL_PLAYER),
	m_statelistener(nullptr),
	m_interpolationtime(interpolationtime),
	m_fullsimulator(fullsimulator),
	m_colmgr(nullptr),
	m_owncharacter(nullptr)
{

}

GameSimulator::~GameSimulator()
{

}

bool GameSimulator::update(float etime)
{
	m_colmgr->update(etime);
	for(std::pair<const sf::Uint16, std::unique_ptr<GameEntity>> &entity : m_entities)
		entity.second->update(etime);
	return true;
}

sf::Uint8 GameSimulator::getOwnId() const
{
	return m_ownid;
}

const Map &GameSimulator::getMap() const
{
	return m_map;
}

void GameSimulator::selfSetDirection(const sf::Vector2f &direction)
{
	if(!m_owncharacter)
	{
#ifndef NDEBUG
		std::cerr << "[DEBUG]selfSetDirection() function called, but no character exists for self." << std::endl;
#endif
		return;
	}
	m_owncharacter->setDirection(direction);
}

Player *GameSimulator::addPlayer(Player &&player)
{
	sf::Uint8 id = player.id;

	assert(id != NEUTRAL_PLAYER);

	auto added = m_players.emplace(id, std::move(player));

	if(!added.second)
	{
		//Player id already exists
#ifndef NDEBUG
		std::cerr << "[DEBUG]Cannot create new player. Id " << (int)id << " already reserved." << std::endl;
#endif
		return nullptr;
	}
	//Tell the listener if required
	if(m_statelistener)
		m_statelistener->onNewPlayer(added.first->second);
#ifndef NDEBUG
	std::cout << "[DEBUG]New player. Id : " << (int)id << ". Name : " << m_players[id].name << ". AI : " << (int)m_players[id].ai << "." << std::endl;
#endif
	return &added.first->second;
}

bool GameSimulator::removePlayer(sf::Uint8 id, sf::Uint8 reason)
{
	assert(id != NEUTRAL_PLAYER);

	auto it = m_players.find(id);
	if(it == m_players.end())
	{
#ifndef NDEBUG
	std::cout << "[DEBUG]Cannot remove player. Player with id " << (int)id << " does not exist." << std::endl;
#endif
		return false;
	}

#ifndef NDEBUG
	std::cout << "[DEBUG]Removing player. Id : " << (int)id << ". Name : " << m_players[id].name << ". Reason : " << (int)reason << "." << std::endl;
#endif

	//Tell the listener if required
	if(m_statelistener)
		m_statelistener->onPlayerLeft(it->second, reason);
	//Remove the player from the table
	m_players.erase(it);
	return true;
}

const Player &GameSimulator::getPlayer(sf::Uint8 id) const
{
	return m_players.at(id);
}

Player &GameSimulator::getPlayer(sf::Uint8 id)
{
	return m_players.at(id);
}

const std::unordered_map<sf::Uint8, Player> &GameSimulator::getPlayers() const
{
	return m_players;
}

bool GameSimulator::playerExists(sf::Uint8 id) const
{
	return m_players.count(id) != 0;
}

bool GameSimulator::removeEntity(sf::Uint16 id)
{
	assert(id != NO_ENTITY_ID);

	auto it = m_entities.find(id);
	if(it == m_entities.end())
	{
#ifndef NDEBUG
	std::cout << "[DEBUG]Cannot remove entity. Entity with id " << (int)id << " does not exist." << std::endl;
#endif
		return false;
	}

#ifndef NDEBUG
	std::cout << "[DEBUG]Removing entity. Id : " << (int)id << "." << std::endl;
#endif

	//Tell the listener if required
	if(m_statelistener)
		m_statelistener->onEntityRemoved(it->second.get());
	onEntityRemoved(it->second.get());
	//Remove the entity from the table
	m_entities.erase(it);
	return true;
}

void GameSimulator::removeEntityLater(sf::Uint16 id)
{
	m_enttoremove.emplace_back(id);
}

const GameEntity *GameSimulator::getEntity(sf::Uint16 id) const
{
	try
	{
		return m_entities.at(id).get();
	}
	catch(const std::out_of_range &)
	{
		return nullptr;
	}
}

GameEntity *GameSimulator::getEntity(sf::Uint16 id)
{
	try
	{
		return m_entities.at(id).get();
	}
	catch(const std::out_of_range &)
	{
		return nullptr;
	}
}

const std::unordered_map<sf::Uint16, std::unique_ptr<GameEntity>> &GameSimulator::getEntities() const
{
	return m_entities;
}

Character *GameSimulator::getOwnCharacter()
{
	return m_owncharacter;
}

const Character *GameSimulator::getOwnCharacter() const
{
	return m_owncharacter;
}

bool GameSimulator::setOwnCharacter(sf::Uint16 id)
{
	if(id == NO_ENTITY_ID)
		m_owncharacter = nullptr;
	else
	{
		//Try to get the character, or return false if this id does not exist
		m_owncharacter = dynamic_cast<Character *>(getEntity(id));
		return m_owncharacter;
	}
	return true;
}

void GameSimulator::getObjectsVisibleFrom(Character *viewer, std::list<CollisionObject *> &visible)
{
	sf::Vector2f pos = viewer->getPosition();
	m_colmgr->getObjectsVisibleFrom(pos.x, pos.y, visible);
}

void GameSimulator::setStateListener(SimulatorStateListener *listener)
{
	m_statelistener = listener;

	if(listener)
	{
		//Tell the listener everything about the current state
		if(m_map)
			listener->onMapLoaded(m_map);
		for(std::pair<const sf::Uint8, Player> &player : m_players)
			listener->onNewPlayer(player.second);
		for(std::pair<const sf::Uint16, std::unique_ptr<GameEntity>> &entity : m_entities)
			listener->onNewEntity(entity.second.get());
	}
}

bool GameSimulator::loadMap(const std::string &name)
{
	if(m_map.load(name))
	{
		//Reload the collision manager if necessary
		m_colmgr = make_unique<DefaultCollisionManager>(m_map);
		m_colmgr->setPostStepCallBack(std::bind(&GameSimulator::removePlannedEntities, this));
		for(std::pair<const sf::Uint16, std::unique_ptr<GameEntity>> &entity : m_entities)
			entity.second->setCollisionManager(m_colmgr.get());
		if(m_statelistener)
			m_statelistener->onMapLoaded(m_map);
		onMapLoaded(name);
		return true;
	}
	return false;
}

void GameSimulator::removePlannedEntities()
{
	//Remove all entities that need to be removed
	for(sf::Uint16 id : m_enttoremove)
		removeEntity(id);
	m_enttoremove.clear();
}

GameEntity *GameSimulator::addNetworkEntity(sf::Uint8 entitytype, sf::Packet &packet)
{
	//Which type of entity was received ?
	switch(entitytype)
	{
		case (sf::Uint8)EntityType::Character:
			return addNetworkCharacter(packet);

		case (sf::Uint8)EntityType::LineDamageSpell:
			return addNetworkLineDamageSpell(packet);

		default:
			break;
	}
	return nullptr;
}

bool GameSimulator::writeEntityInitData(GameEntity *entity, sf::Packet &packet, bool hideserverinfo)
{
	//Add the init data depending on the type of entity
	if(Character *character = dynamic_cast<Character *>(entity))
	{
		packet << (sf::Uint8)EntityType::Character;
		writeCharacterInitData(character, packet, hideserverinfo);
		return true;
	}
	else if(LineDamageSpell *spell = dynamic_cast<LineDamageSpell *>(entity))
	{
		packet << (sf::Uint8)EntityType::LineDamageSpell;
		writeLineDamageSpellInitData(spell, packet, hideserverinfo);
		return true;
	}
	//Unknown entity type
#ifndef NDEBUG
	std::cout << "[DEBUG]Warning : Cannot add entity of unknown type to packet." << std::endl;
#endif
	return false;
}

Character *GameSimulator::addNetworkCharacter(sf::Packet &packet)
{
	float x, y;
	sf::Uint16 id;
	sf::Uint8 owner;
	sf::Uint8 state;
	//Extract the data
	if(!(packet >> id >> owner >> x >> y >> state))
		return nullptr;
	//Can the state be safely casted to State enum type ?
	if(state >= (sf::Uint8)Character::State::Count)
		return nullptr;
	//Add the character and initialize it
	Character *character = addEntity<Character>(*this, id, static_cast<Character::State>(state));
	if(character)
	{
		character->forcePosition(x, y);
		character->setOwner(owner);
	}
	return character;
}

void GameSimulator::writeCharacterInitData(Character *character, sf::Packet &packet, bool hideserverinfo)
{
	//Write the character data into the packet
	const sf::Vector2f position = character->getPosition();
	packet << (sf::Uint16)character->getId()
	       << (sf::Uint8)(hideserverinfo ? NEUTRAL_PLAYER : character->getOwner())
	       << position.x
	       << position.y
	       << (sf::Uint8)character->getState();
}

LineDamageSpell *GameSimulator::addNetworkLineDamageSpell(sf::Packet &packet)
{
	sf::Uint16 id;
	sf::Uint8 owner;
	sf::Uint16 appearance;
	if(!(packet >> id >> owner >> appearance))
		return nullptr;
	LineDamageSpell *spell = addEntity<LineDamageSpell>(*this, id, appearance, nullptr, 0.f, 0);
	if(spell)
		spell->setOwner(owner);
	return spell;
}

void GameSimulator::writeLineDamageSpellInitData(LineDamageSpell *spell, sf::Packet &packet, bool hideserverinfo)
{
	packet << (sf::Uint16)spell->getId()
	       << (sf::Uint8)(hideserverinfo ? NEUTRAL_PLAYER : spell->getOwner())
	       << (sf::Uint16)spell->getAppearance();
}
