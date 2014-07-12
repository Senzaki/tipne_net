#ifndef GAMESIMULATOR_HPP_INCLUDED
#define GAMESIMULATOR_HPP_INCLUDED

#include "RoundState.hpp"
#include <SFML/System.hpp>
#include <string>
#include "Player.hpp"
#include "NetworkCodes.hpp"
#include "SimulatorStateListener.hpp"
#include "Map.hpp"

class Spell;

class GameSimulator
{
	public:
	GameSimulator(bool fullsimulator, float interpolationtime);
	virtual ~GameSimulator();

	GameSimulator(const GameSimulator &) = delete;
	GameSimulator &operator=(const GameSimulator &) = delete;

	virtual bool update(float etime);

	void setStateListener(SimulatorStateListener *listener);//nullptr to remove the current simulator

	const Player &getPlayer(sf::Uint8 id) const;//Throws std::out_of_range if no player corresponds to this id.
	Player &getPlayer(sf::Uint8 id);//Throws std::out_of_range if no player corresponds to this id.
	bool playerExists(sf::Uint8 id) const;

	sf::Uint8 getOwnId() const;//Will return NEUTRAL_PLAYER if no id

	const Map &getMap() const;

	//Override to prevent full-authority (e.g. in a client, only send a request)
	virtual void selfSetDirection(const sf::Vector2f &direction);
	virtual void selfCastSpell(const Spell &spell) = 0;

	RoundState &getRoundState();
	const RoundState &getRoundState() const;
	bool startNewRound(const std::string &mapname = std::string());//If mapname is empty, use the same map

	protected:
	template<typename... Args>
	inline Player *addPlayer(Args &&...args);
	Player *addPlayer(Player &&player);
	bool removePlayer(sf::Uint8 id, sf::Uint8 reason = (sf::Uint8)DisconnectionReason::Left);
	const std::unordered_map<sf::Uint8, Player> &getPlayers() const;

	virtual void onEntityAdded(GameEntity *entity);
	virtual void onEntityRemoved(GameEntity *entity);
	virtual void onNewRoundStarted(const std::string &mapname);

	sf::Uint8 m_ownid;
	SimulatorStateListener *m_statelistener;
	std::unique_ptr<RoundState> m_round;

	private:
	float m_interpolationtime;
	bool m_fullsimulator;

	std::unordered_map<sf::Uint8, Player> m_players;
	Map m_map;

	friend class RoundState;
};

#include "GameSimulator.inl"

#endif // GAMESIMULATOR_HPP_INCLUDED
