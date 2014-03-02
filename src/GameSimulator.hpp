#ifndef GAMESIMULATOR_HPP_INCLUDED
#define GAMESIMULATOR_HPP_INCLUDED

#include <SFML/System.hpp>
#include <string>
#include <unordered_map>
#include "Player.hpp"
#include "NetworkCodes.hpp"
#include "SimulatorStateListener.hpp"
#include "Map.hpp"

class GameSimulator
{
	public:
	GameSimulator();
	virtual ~GameSimulator();

	GameSimulator(const GameSimulator &) = delete;
	GameSimulator &operator=(const GameSimulator &) = delete;

	virtual bool update(float etime);

	void setStateListener(SimulatorStateListener *listener);//nullptr to remove the current simulator

	const Player &getPlayer(sf::Uint8 id) const;//May throw std::out_of_range if no player corresponds to this id.
	Player &getPlayer(sf::Uint8 id);//May throw std::out_of_range if no player corresponds to this id.
	bool playerExists(sf::Uint8 id) const;

	sf::Uint8 getOwnId() const;//Will return NEUTRAL_PLAYER if no id

	const Map &getMap() const;

	protected:
	bool addPlayer(sf::Uint8 id, const std::string &name, bool ai = false);
	bool addPlayer(Player &&player);
	bool removePlayer(sf::Uint8 id, sf::Uint8 reason = (sf::Uint8)DisconnectionReason::Left);
	const std::unordered_map<sf::Uint8, Player> &getPlayers() const;

	virtual bool loadMap(sf::Uint8 mapid);

	sf::Uint8 m_ownid;

	private:
	std::unordered_map<sf::Uint8, Player> m_players;
	SimulatorStateListener *m_statelistener;
	Map m_map;
};

#endif // GAMESIMULATOR_HPP_INCLUDED
