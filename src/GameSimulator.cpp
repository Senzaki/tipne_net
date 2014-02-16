#include "GameSimulator.hpp"
#include <cassert>

GameSimulator::GameSimulator():
	m_ownid(NEUTRAL_PLAYER),
	m_statelistener(nullptr)
{

}

GameSimulator::~GameSimulator()
{

}

void GameSimulator::update(float etime)
{

}

sf::Uint8 GameSimulator::getOwnId() const
{
	return m_ownid;
}

void GameSimulator::addPlayer(Player &&player)
{
	sf::Uint8 id = player.id;
	assert(m_players.count(id) == 0);

	m_players.emplace(id, std::move(player));
	//Tell the listener if required
	if(m_statelistener)
		m_statelistener->onNewPlayer(m_players[id]);
}

void GameSimulator::addPlayer(sf::Uint8 id, const std::string &name, bool ai)
{
	assert(m_players.count(id) == 0);

	m_players.emplace(id, Player(id, name, ai));
	//Tell the listener if required
	if(m_statelistener)
		m_statelistener->onNewPlayer(m_players[id]);
}

void GameSimulator::removePlayer(sf::Uint8 id, sf::Uint8 reason)
{
	assert(m_players.count(id) != 0);

	//Tell the listener if required
	if(m_statelistener)
		m_statelistener->onPlayerLeft(m_players[id], reason);
	//Remove the player from the table
	m_players.erase(id);
}

const Player &GameSimulator::getPlayer(sf::Uint8 id) const
{
	return m_players.at(id);
}

Player &GameSimulator::getPlayer(sf::Uint8 id)
{
	return m_players.at(id);
}

void GameSimulator::setStateListener(SimulatorStateListener *listener)
{
	m_statelistener = listener;
}

const std::unordered_map<sf::Uint8, Player> &GameSimulator::getPlayers() const
{
	return m_players;
}
