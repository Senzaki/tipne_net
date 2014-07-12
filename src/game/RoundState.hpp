#ifndef ROUNDSTATE_HPP_INCLUDED
#define ROUNDSTATE_HPP_INCLUDED

#include "CollisionManager.hpp"
#include "Character.hpp"
#include "LineDamageSpell.hpp"
#include <SFML/Network.hpp>
#include <unordered_map>
#include <list>
#include <memory>

class GameSimulator;

class RoundState
{
	public:
	RoundState(GameSimulator &simulator);

	RoundState(const RoundState &) = delete;
	RoundState &operator=(const RoundState &) = delete;

	void update(float etime);

	const GameSimulator &getSimulator() const;
	GameSimulator &getSimulator();

	const GameEntity *getEntity(sf::Uint16 id) const;//Returns nullptr if no entity corresponds to this id.
	GameEntity *getEntity(sf::Uint16 id);//Returns nullptr if no entity corresponds to this id.

	template<typename EntityType, typename... Args>
	EntityType *addEntity(Args &&...args);
	bool removeEntity(sf::Uint16 id);
	void removeEntityLater(sf::Uint16 id);
	void removePlannedEntities();

	const Character *getOwnCharacter() const;
	Character *getOwnCharacter();
	void setOwnCharacter(Character *character);
	bool setOwnCharacter(sf::Uint16 id);

	const std::unordered_map<sf::Uint16, std::unique_ptr<GameEntity>> &getEntities() const;
	std::unordered_map<sf::Uint16, std::unique_ptr<GameEntity>>::iterator getEntitiesBeginIterator();
	std::unordered_map<sf::Uint16, std::unique_ptr<GameEntity>>::iterator getEntitiesEndIterator();

	void getObjectsVisibleFrom(Character *viewer, std::list<CollisionObject *> &visible);

	GameEntity *addNetworkEntity(sf::Uint8 entitytype, sf::Packet &packet);
	//Forbid implicit conversion
	template<typename T>
	GameEntity *addNetworkEntity(T entitytype, sf::Packet &packet) = delete;
	static bool writeEntityInitData(GameEntity *entity, sf::Packet &packet, bool hideserverinfo);

	private:
	Character *addNetworkCharacter(sf::Packet &packet);
	static void writeCharacterInitData(Character *character, sf::Packet &packet, bool hideserverinfo);
	LineDamageSpell *addNetworkLineDamageSpell(sf::Packet &packet);
	static void writeLineDamageSpellInitData(LineDamageSpell *spell, sf::Packet &packet, bool hideserverinfo);

	void setupNewEntity(GameEntity *entity);

	GameSimulator &m_simulator;
	std::unique_ptr<CollisionManager> m_colmgr;

	std::unordered_map<sf::Uint16, std::unique_ptr<GameEntity>> m_entities;
	Character *m_owncharacter;
	std::list<sf::Uint16> m_enttoremove;
};

#include "RoundState.inl"

#endif // ROUNDSTATE_HPP_INCLUDED
