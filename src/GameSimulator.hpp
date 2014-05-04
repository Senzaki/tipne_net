#ifndef GAMESIMULATOR_HPP_INCLUDED
#define GAMESIMULATOR_HPP_INCLUDED

#include <SFML/System.hpp>
#include <string>
#include <unordered_map>
#include "Player.hpp"
#include "NetworkCodes.hpp"
#include "SimulatorStateListener.hpp"
#include "Map.hpp"
#include "Character.hpp"
#include <list>

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

	const Character &getCharacter(sf::Uint16 id) const;//Throws std::out_of_range if no player corresponds to this id.
	Character &getCharacter(sf::Uint16 id);//Throws std::out_of_range if no player corresponds to this id.
	bool characterExists(sf::Uint16 id);

	sf::Uint8 getOwnId() const;//Will return NEUTRAL_PLAYER if no id
	const Character *getOwnCharacter() const;

	const Map &getMap() const;

	//Override to prevent full-authority (e.g. in a client, only send a request)
	virtual void selfSetDirection(const sf::Vector2f &direction);

	protected:
	template<typename... Args>
	inline Player *addPlayer(Args &&...args);
	Player *addPlayer(Player &&player);
	virtual bool removePlayer(sf::Uint8 id, sf::Uint8 reason = (sf::Uint8)DisconnectionReason::Left);
	const std::unordered_map<sf::Uint8, Player> &getPlayers() const;

	template<typename... Args>
	inline Character *addCharacter(Args &&...args);
	Character *addCharacter(Character &&character);
	virtual bool removeCharacter(sf::Uint16 id);
	const std::unordered_map<sf::Uint16, Character> &getCharacters() const;
	Character *getOwnCharacter();
	bool setOwnCharacter(sf::Uint16 id);

	void getObjectsVisibleFrom(Character *viewer, std::list<CollisionObject *> &visible);

	virtual bool loadMap(const std::string &name);

	sf::Uint8 m_ownid;
	SimulatorStateListener *m_statelistener;

	private:
	float m_interpolationtime;
	bool m_fullsimulator;

	std::unordered_map<sf::Uint8, Player> m_players;
	Map m_map;
	CollisionManager *m_colmgr;

	std::unordered_map<sf::Uint16, Character> m_characters;
	Character *m_owncharacter;
};

#include "GameSimulator.inl"

#endif // GAMESIMULATOR_HPP_INCLUDED
