#ifndef GAMESIMULATOR_HPP_INCLUDED
#define GAMESIMULATOR_HPP_INCLUDED

#include <SFML/System.hpp>
#include <string>
#include <unordered_map>
#include "Player.hpp"
#include "NetworkCodes.hpp"
#include "SimulatorStateListener.hpp"
#include "Map.hpp"
#include "Spell.hpp"
#include <list>

class Character;
class LineDamageSpell;

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

	const GameEntity *getEntity(sf::Uint16 id) const;//Returns nullptr if no entity corresponds to this id.
	GameEntity *getEntity(sf::Uint16 id);//Returns nullptr if no entity corresponds to this id.

	template<typename EntityType, typename... Args>
	EntityType *addEntity(Args &&...args);
	bool removeEntity(sf::Uint16 id);
	void removeEntityLater(sf::Uint16 id);

	sf::Uint8 getOwnId() const;//Will return NEUTRAL_PLAYER if no id
	const Character *getOwnCharacter() const;

	const Map &getMap() const;

	//Override to prevent full-authority (e.g. in a client, only send a request)
	virtual void selfSetDirection(const sf::Vector2f &direction);
	virtual void selfCastSpell(const Spell &spell) = 0;

	bool loadMap(const std::string &name);

	protected:
	template<typename... Args>
	inline Player *addPlayer(Args &&...args);
	Player *addPlayer(Player &&player);
	bool removePlayer(sf::Uint8 id, sf::Uint8 reason = (sf::Uint8)DisconnectionReason::Left);
	const std::unordered_map<sf::Uint8, Player> &getPlayers() const;

	const std::unordered_map<sf::Uint16, GameEntity *> &getEntities() const;

	Character *getOwnCharacter();
	bool setOwnCharacter(sf::Uint16 id);

	void getObjectsVisibleFrom(Character *viewer, std::list<CollisionObject *> &visible);

	virtual void onMapLoaded(const std::string &name) { }
	virtual void onEntityAdded(GameEntity *entity) { }
	virtual void onEntityRemoved(GameEntity *entity) { }

	GameEntity *addNetworkEntity(sf::Uint8 entitytype, sf::Packet &packet);
	//Forbid implicit conversion
	template<typename T>
	GameEntity *addNetworkEntity(T entitytype, sf::Packet &packet) = delete;
	static bool writeEntityInitData(GameEntity *entity, sf::Packet &packet, bool hideserverinfo);

	sf::Uint8 m_ownid;
	SimulatorStateListener *m_statelistener;

	private:
	bool addEntity(GameEntity *entity);

	Character *addNetworkCharacter(sf::Packet &packet);
	static void writeCharacterInitData(Character *character, sf::Packet &packet, bool hideserverinfo);
	LineDamageSpell *addNetworkLineDamageSpell(sf::Packet &packet);
	static void writeLineDamageSpellInitData(LineDamageSpell *spell, sf::Packet &packet, bool hideserverinfo);

	float m_interpolationtime;
	bool m_fullsimulator;

	std::unordered_map<sf::Uint8, Player> m_players;
	Map m_map;
	CollisionManager *m_colmgr;

	std::unordered_map<sf::Uint16, GameEntity *> m_entities;
	Character *m_owncharacter;
	std::list<sf::Uint16> m_enttoremove;
};

#include "GameSimulator.inl"

#endif // GAMESIMULATOR_HPP_INCLUDED
