#ifndef GAMESIMULATOR_HPP_INCLUDED
#define GAMESIMULATOR_HPP_INCLUDED

#include <SFML/System.hpp>
#include <string>
#include <unordered_map>
#include "Player.hpp"
#include "SimulatorStateListener.hpp"
#include "IDCreator.hpp"

class GameSimulator
{
	public:
	GameSimulator();
	virtual ~GameSimulator();

	virtual void update(float etime);

	sf::Uint8 addPlayer(std::string name);
	void removePlayer(sf::Uint8 id);
	const Player &getPlayer(sf::Uint8 id) const;
	Player &getPlayer(sf::Uint8 id);

	void setListener(SimulatorStateListener *m_listener);//nullptr to remove the current simulator

	private:
	IDCreator<sf::Uint8> m_playersids;
	std::unordered_map<sf::Uint8, Player *> m_players;
	SimulatorStateListener *m_listener;
};

#endif // GAMESIMULATOR_HPP_INCLUDED
