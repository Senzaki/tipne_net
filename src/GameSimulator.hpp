#ifndef GAMESIMULATOR_HPP_INCLUDED
#define GAMESIMULATOR_HPP_INCLUDED

#include <SFML/System.hpp>
#include <string>
#include <unordered_map>
#include "Player.hpp"
#include "SimulatorStateListener.hpp"

class GameSimulator
{
	public:
	GameSimulator();
	virtual ~GameSimulator();

	GameSimulator(const GameSimulator &) = delete;
	GameSimulator &operator=(const GameSimulator &) = delete;

	virtual void update(float etime);

	void setStateListener(SimulatorStateListener *listener);//nullptr to remove the current simulator

	protected:
	void addPlayer(sf::Uint8 id, const std::string &name, bool ai = false);
	void addPlayer(Player &&player);
	void removePlayer(sf::Uint8 id);
	const Player &getPlayer(sf::Uint8 id) const;//May throw std::out_of_range if no player corresponds to this id.
	Player &getPlayer(sf::Uint8 id);//May throw std::out_of_range if no player corresponds to this id.
	const std::unordered_map<sf::Uint8, Player> &getPlayers() const;

	private:
	std::unordered_map<sf::Uint8, Player> m_players;
	SimulatorStateListener *m_statelistener;
};

#endif // GAMESIMULATOR_HPP_INCLUDED
