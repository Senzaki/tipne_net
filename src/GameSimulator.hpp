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
	GameSimulator();
	virtual ~GameSimulator();

	GameSimulator(const GameSimulator &) = delete;
	GameSimulator &operator=(const GameSimulator &) = delete;

	virtual bool update(float etime);

	void setStateListener(SimulatorStateListener *listener);//nullptr to remove the current simulator

	const Player &getPlayer(sf::Uint8 id) const;//May throw std::out_of_range if no player corresponds to this id.
	Player &getPlayer(sf::Uint8 id);//May throw std::out_of_range if no player corresponds to this id.
	bool playerExists(sf::Uint8 id) const;

	const Character &getCharacter(sf::Uint16 id) const;
	Character &getCharacter(sf::Uint16 id);
	bool characterExists(sf::Uint16 id);

	sf::Uint8 getOwnId() const;//Will return NEUTRAL_PLAYER if no id

	const Map &getMap() const;

	protected:
	template<typename... Args>
	inline bool addPlayer(Args &&...args);
	bool addPlayer(Player &&player);
	virtual bool removePlayer(sf::Uint8 id, sf::Uint8 reason = (sf::Uint8)DisconnectionReason::Left);
	const std::unordered_map<sf::Uint8, Player> &getPlayers() const;

	template<typename... Args>
	inline bool addCharacter(Args &&...args);
	bool addCharacter(Character &&character);
	virtual bool removeCharacter(sf::Uint16 id);
	const std::unordered_map<sf::Uint16, Character> &getCharacters() const;
	Character *getOwnCharacter();
	const Character *getOwnCharacter() const;
	bool setOwnCharacter(sf::Uint16 id);

	virtual bool loadMap(sf::Uint8 mapid);

	sf::Uint8 m_ownid;

	private:
	std::unordered_map<sf::Uint8, Player> m_players;
	Map m_map;

	std::unordered_map<sf::Uint16, Character> m_characters;
	Character *m_owncharacter;

	SimulatorStateListener *m_statelistener;
};

#include "GameSimulator.inl"

#endif // GAMESIMULATOR_HPP_INCLUDED
