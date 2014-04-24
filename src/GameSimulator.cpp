#include "GameSimulator.hpp"
#include <iostream>
#include <cassert>

GameSimulator::GameSimulator(bool collisions):
	m_ownid(NEUTRAL_PLAYER),
	m_colmgr(nullptr),
	m_collisions(collisions),
	m_owncharacter(nullptr),
	m_statelistener(nullptr)
{

}

GameSimulator::~GameSimulator()
{
	delete m_colmgr;
}

bool GameSimulator::update(float etime)
{
	for(std::pair<const sf::Uint16, Character> &character : m_characters)
		character.second.update(etime);
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

Character *GameSimulator::addCharacter(Character &&character)
{
	sf::Uint16 id = character.getId();

	assert(id != NO_CHARACTER_ID);

	auto added = m_characters.emplace(id, std::move(character));

	if(!added.second)
	{
		//Player id already exists
#ifndef NDEBUG
		std::cerr << "[DEBUG]Cannot create new character. Id " << (int)id << " already reserved." << std::endl;
#endif
		return nullptr;
	}
	added.first->second.setSimulator(this);
	//Add it to the collision manager
	added.first->second.setCollisionManager(m_colmgr);
	//Tell the listener if required
	if(m_statelistener)
		m_statelistener->onNewCharacter(m_characters[id]);
#ifndef NDEBUG
	std::cout << "[DEBUG]New character. Id : " << (int)id << "." << std::endl;
#endif
	return &added.first->second;
}

bool GameSimulator::removeCharacter(sf::Uint16 id)
{
	assert(id != NO_CHARACTER_ID);

	auto it = m_characters.find(id);
	if(it == m_characters.end())
	{
#ifndef NDEBUG
	std::cout << "[DEBUG]Cannot remove character. Character with id " << (int)id << " does not exist." << std::endl;
#endif
		return false;
	}

#ifndef NDEBUG
	std::cout << "[DEBUG]Removing character. Id : " << (int)id << "." << std::endl;
#endif

	//Tell the listener if required
	if(m_statelistener)
		m_statelistener->onCharacterRemoved(it->second);
	//Remove the character from the table
	m_characters.erase(it);
	return true;
}

const Character &GameSimulator::getCharacter(sf::Uint16 id) const
{
	return m_characters.at(id);
}

Character &GameSimulator::getCharacter(sf::Uint16 id)
{
	return m_characters.at(id);
}

const std::unordered_map<sf::Uint16, Character> &GameSimulator::getCharacters() const
{
	return m_characters;
}

bool GameSimulator::characterExists(sf::Uint16 id)
{
	return m_characters.count(id) != 0;
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
	if(id == NO_CHARACTER_ID)
		m_owncharacter = nullptr;
	else
	{
		//Try to get the character, or return false if this id does not exist
		try
		{
			m_owncharacter = &m_characters.at(id);
		}
		catch(const std::out_of_range &)
		{
			return false;
		}
	}
	return true;
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
		for(std::pair<const sf::Uint16, Character> &character : m_characters)
			listener->onNewCharacter(character.second);
	}
}

bool GameSimulator::loadMap(const std::string &name)
{
	if(m_map.load(name))
	{
		if(m_collisions)
		{
			//Reload the collision manager if necessary
			delete m_colmgr;
			m_colmgr = new CollisionManager(m_map);
			for(std::pair<const sf::Uint16, Character> &character : m_characters)
				character.second.setCollisionManager(m_colmgr);
		}
		if(m_statelistener)
			m_statelistener->onMapLoaded(m_map);
		return true;
	}
	return false;
}
