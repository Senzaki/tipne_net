#include "GameSimulator.hpp"
#include <iostream>

GameSimulator::GameSimulator():
	m_ownid(NEUTRAL_PLAYER),
	m_statelistener(nullptr)
{

}

GameSimulator::~GameSimulator()
{

}

bool GameSimulator::update(float etime)
{
	return true;
}

sf::Uint8 GameSimulator::getOwnId() const
{
	return m_ownid;
}

bool GameSimulator::addPlayer(Player &&player)
{
	sf::Uint8 id = player.id;

	if(!m_players.emplace(id, std::move(player)).second)
	{
		//Player id already exists
#ifndef NDEBUG
		std::cerr << "[DEBUG]Cannot create new player. Id " << (int)id << " already reserved." << std::endl;
#endif
		return false;
	}
	//Tell the listener if required
	if(m_statelistener)
		m_statelistener->onNewPlayer(m_players[id]);
#ifndef NDEBUG
	std::cout << "[DEBUG]New player. Id : " << (int)id << ". Name : " << m_players[id].name << ". AI : " << (int)m_players[id].ai << "." << std::endl;
#endif
	return true;
}

bool GameSimulator::addPlayer(sf::Uint8 id, const std::string &name, bool ai)
{
	if(!m_players.emplace(id, Player(id, name, ai)).second)
	{
		//Player id already exists
#ifndef NDEBUG
		std::cerr << "[DEBUG]Cannot create new player. Id " << (int)id << " already reserved." << std::endl;
#endif
		return false;
	}
	//Tell the listener if required
	if(m_statelistener)
		m_statelistener->onNewPlayer(m_players[id]);
#ifndef NDEBUG
	std::cout << "[DEBUG]New player. Id : " << (int)id << ". Name : " << m_players[id].name << ". AI : " << (int)m_players[id].ai << "." << std::endl;
#endif
	return true;
}

bool GameSimulator::removePlayer(sf::Uint8 id, sf::Uint8 reason)
{
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

bool GameSimulator::playerExists(sf::Uint8 id) const
{
	return m_players.count(id) != 0;
}

void GameSimulator::setStateListener(SimulatorStateListener *listener)
{
	m_statelistener = listener;
}

const std::unordered_map<sf::Uint8, Player> &GameSimulator::getPlayers() const
{
	return m_players;
}
