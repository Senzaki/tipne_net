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
	m_fullsimulator(fullsimulator)
{
	startNewRound();
}

GameSimulator::~GameSimulator()
{

}

bool GameSimulator::update(float etime)
{
	m_round->update(etime);
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
	Character *owncharacter = m_round->getOwnCharacter();
	if(!owncharacter)
	{
#ifndef NDEBUG
		std::cerr << "[DEBUG]selfSetDirection() function called, but no character exists for self." << std::endl;
#endif
		return;
	}
	owncharacter->setDirection(direction);
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

void GameSimulator::onEntityAdded(GameEntity *entity)
{
	//Tell the listener if required
	if(m_statelistener)
		m_statelistener->onNewEntity(entity);
}

void GameSimulator::onEntityRemoved(GameEntity *entity)
{
	//Tell the listener if required
	if(m_statelistener)
		m_statelistener->onEntityRemoved(entity);
}
void GameSimulator::onNewRoundStarted(const std::string &mapname)
{
	//Tell the listener if required
	if(m_statelistener)
		m_statelistener->onNewRoundStarted(m_map);
}

void GameSimulator::setStateListener(SimulatorStateListener *listener)
{
	m_statelistener = listener;

	if(listener)
	{
		//Tell the listener everything about the current state
		if(m_map)
			listener->onNewRoundStarted(m_map);
		for(std::pair<const sf::Uint8, Player> &player : m_players)
			listener->onNewPlayer(player.second);
		for(auto it = m_round->getEntitiesBeginIterator(); it != m_round->getEntitiesEndIterator(); it++)
			listener->onNewEntity(it->second.get());
	}
}

RoundState &GameSimulator::getRoundState()
{
	assert(m_round);
	return *m_round;
}

const RoundState &GameSimulator::getRoundState() const
{
	assert(m_round);
	return *m_round;
}

bool GameSimulator::startNewRound(const std::string &mapname)
{
	//Delete the current round
	m_round.reset();
	//Load the new map if necessary
	if(mapname.empty())
	{
		if(!m_map)
		{
			std::cerr << "Round started with previous map, but no map was loaded." << std::endl;
			return false;
		}
	}
	else if(!m_map.load(mapname))
	{
		std::cerr << "Cannot load the requested map." << std::endl;
		return false;
	}
	//Reload the round
	m_round = make_unique<RoundState>(*this);
	onNewRoundStarted(mapname);
	return true;
}
