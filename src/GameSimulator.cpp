#include "GameSimulator.hpp"
#include <cassert>

GameSimulator::GameSimulator():
	m_statelistener(nullptr)
{

}

GameSimulator::~GameSimulator()
{
	for(std::pair<const sf::Uint8, Player *> &player : m_players)
		delete player.second;
}

void GameSimulator::update(float etime)
{

}

void GameSimulator::addPlayer(Player &&player)
{
	sf::Uint8 id = player.id;
	assert(m_players.count(id) == 0);

	m_players[id] = new Player(std::move(player));
	//Tell the listener if required
	if(m_statelistener)
		m_statelistener->onNewPlayer(*m_players[id]);
}

void GameSimulator::addPlayer(sf::Uint8 id, const std::string &name, bool ai)
{
	assert(m_players.count(id) == 0);

	m_players[id] = new Player(id, name, ai);
	//Tell the listener if required
	if(m_statelistener)
		m_statelistener->onNewPlayer(*m_players[id]);
}

void GameSimulator::removePlayer(sf::Uint8 id)
{
	assert(m_players.count(id) != 0);

	//Tell the listener if required
	if(m_statelistener)
		m_statelistener->onPlayerLeft(*m_players[id]);
	//Remove the player from the table
	delete m_players[id];
	m_players.erase(id);
}

const Player &GameSimulator::getPlayer(sf::Uint8 id) const
{
	return *m_players.at(id);
}

Player &GameSimulator::getPlayer(sf::Uint8 id)
{
	return *m_players.at(id);
}

void GameSimulator::setStateListener(SimulatorStateListener *listener)
{
	m_statelistener = listener;
}

const std::unordered_map<sf::Uint8, Player *> &GameSimulator::getPlayers() const
{
	return m_players;
}
