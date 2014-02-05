#include "GameSimulator.hpp"

GameSimulator::GameSimulator():
	m_listener(nullptr)
{

}

GameSimulator::~GameSimulator()
{

}


void GameSimulator::update(float etime)
{

}

sf::Uint8 GameSimulator::addPlayer(std::string name)
{
	sf::Uint8 id = m_playersids.getNewID();
	m_players[id] = new Player{id, name};
	if(m_listener)
		m_listener->onNewPlayer(*m_players[id]);
	return id;
}

void GameSimulator::removePlayer(sf::Uint8 id)
{
	try
	{
		m_players.at(id);
	}
	catch(const std::out_of_range &)
	{
		return;
	}

	if(m_listener)
		m_listener->onPlayerLeft(*m_players[id]);
	delete m_players[id];
	m_players.erase(id);
	m_playersids.releaseID(id);
}

const Player &GameSimulator::getPlayer(sf::Uint8 id) const
{
	return *m_players.at(id);
}

Player &GameSimulator::getPlayer(sf::Uint8 id)
{
	return *m_players.at(id);
}

void GameSimulator::setListener(SimulatorStateListener *listener)
{
	m_listener = listener;
}
