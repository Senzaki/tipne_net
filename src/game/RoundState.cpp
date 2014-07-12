#include "RoundState.hpp"
#include "GameSimulator.hpp"
#include "DefaultCollisionManager.hpp"
#include <cassert>
#include <iostream>

RoundState::RoundState(GameSimulator &simulator):
	m_simulator(simulator),
	m_colmgr(make_unique<DefaultCollisionManager>(simulator.getMap()))
{
	m_colmgr->setPostStepCallBack(std::bind(&RoundState::removePlannedEntities, this));
}

void RoundState::update(float etime)
{
	m_colmgr->update(etime);
	for(std::pair<const sf::Uint16, std::unique_ptr<GameEntity>> &entity : m_entities)
		entity.second->update(etime);
}

bool RoundState::removeEntity(sf::Uint16 id)
{
	assert(id != NO_ENTITY_ID);

	auto it = m_entities.find(id);
	if(it == m_entities.end())
	{
#ifndef NDEBUG
	std::cout << "[DEBUG]Cannot remove entity. Entity with id " << (int)id << " does not exist." << std::endl;
#endif
		return false;
	}

#ifndef NDEBUG
	std::cout << "[DEBUG]Removing entity. Id : " << (int)id << "." << std::endl;
#endif

	m_simulator.onEntityRemoved(it->second.get());
	//Remove the entity from the table
	m_entities.erase(it);
	return true;
}

void RoundState::removeEntityLater(sf::Uint16 id)
{
	m_enttoremove.emplace_back(id);
}

void RoundState::removePlannedEntities()
{
	//Remove all entities that need to be removed
	for(sf::Uint16 id : m_enttoremove)
		removeEntity(id);
	m_enttoremove.clear();
}

const GameEntity *RoundState::getEntity(sf::Uint16 id) const
{
	try
	{
		return m_entities.at(id).get();
	}
	catch(const std::out_of_range &)
	{
		return nullptr;
	}
}

GameEntity *RoundState::getEntity(sf::Uint16 id)
{
	try
	{
		return m_entities.at(id).get();
	}
	catch(const std::out_of_range &)
	{
		return nullptr;
	}
}

const std::unordered_map<sf::Uint16, std::unique_ptr<GameEntity>> &RoundState::getEntities() const
{
	return m_entities;
}

std::unordered_map<sf::Uint16, std::unique_ptr<GameEntity>>::iterator RoundState::getEntitiesBeginIterator()
{
	return m_entities.begin();
}

std::unordered_map<sf::Uint16, std::unique_ptr<GameEntity>>::iterator RoundState::getEntitiesEndIterator()
{
	return m_entities.end();
}

Character *RoundState::getOwnCharacter()
{
	return m_owncharacter;
}

const Character *RoundState::getOwnCharacter() const
{
	return m_owncharacter;
}

void RoundState::setOwnCharacter(Character *character)
{
	m_owncharacter = character;
}

bool RoundState::setOwnCharacter(sf::Uint16 id)
{
	if(id == NO_ENTITY_ID)
		m_owncharacter = nullptr;
	else
	{
		//Try to get the character, or return false if this id does not exist (or if the entity is not a character)
		m_owncharacter = dynamic_cast<Character *>(getEntity(id));
		return m_owncharacter;
	}
	return true;
}

void RoundState::getObjectsVisibleFrom(Character *viewer, std::list<CollisionObject *> &visible)
{
	sf::Vector2f pos = viewer->getPosition();
	m_colmgr->getObjectsVisibleFrom(pos.x, pos.y, visible);
}

GameEntity *RoundState::addNetworkEntity(sf::Uint8 entitytype, sf::Packet &packet)
{
	//Which type of entity was received ?
	switch(entitytype)
	{
		case (sf::Uint8)EntityType::Character:
			return addNetworkCharacter(packet);

		case (sf::Uint8)EntityType::LineDamageSpell:
			return addNetworkLineDamageSpell(packet);

		default:
			break;
	}
	return nullptr;
}

bool RoundState::writeEntityInitData(GameEntity *entity, sf::Packet &packet, bool hideserverinfo)
{
	//Add the init data depending on the type of entity
	if(Character *character = dynamic_cast<Character *>(entity))
	{
		packet << (sf::Uint8)EntityType::Character;
		writeCharacterInitData(character, packet, hideserverinfo);
		return true;
	}
	else if(LineDamageSpell *spell = dynamic_cast<LineDamageSpell *>(entity))
	{
		packet << (sf::Uint8)EntityType::LineDamageSpell;
		writeLineDamageSpellInitData(spell, packet, hideserverinfo);
		return true;
	}
	//Unknown entity type
#ifndef NDEBUG
	std::cout << "[DEBUG]Warning : Cannot add entity of unknown type to packet." << std::endl;
#endif
	return false;
}

Character *RoundState::addNetworkCharacter(sf::Packet &packet)
{
	float x, y;
	sf::Uint16 id;
	sf::Uint8 owner;
	sf::Uint8 state;
	//Extract the data
	if(!(packet >> id >> owner >> x >> y >> state))
		return nullptr;
	//Can the state be safely casted to State enum type ?
	if(state >= (sf::Uint8)Character::State::Count)
		return nullptr;
	//Add the character and initialize it
	Character *character = addEntity<Character>(*this, id, static_cast<Character::State>(state));
	if(character)
	{
		character->forcePosition(x, y);
		character->setOwner(owner);
	}
	return character;
}

void RoundState::writeCharacterInitData(Character *character, sf::Packet &packet, bool hideserverinfo)
{
	//Write the character data into the packet
	const sf::Vector2f position = character->getPosition();
	packet << (sf::Uint16)character->getId()
	       << (sf::Uint8)(hideserverinfo ? NEUTRAL_PLAYER : character->getOwner())
	       << position.x
	       << position.y
	       << (sf::Uint8)character->getState();
}

LineDamageSpell *RoundState::addNetworkLineDamageSpell(sf::Packet &packet)
{
	sf::Uint16 id;
	sf::Uint8 owner;
	sf::Uint16 appearance;
	if(!(packet >> id >> owner >> appearance))
		return nullptr;
	LineDamageSpell *spell = addEntity<LineDamageSpell>(*this, id, appearance, nullptr, 0.f, 0);
	if(spell)
		spell->setOwner(owner);
	return spell;
}

void RoundState::writeLineDamageSpellInitData(LineDamageSpell *spell, sf::Packet &packet, bool hideserverinfo)
{
	packet << (sf::Uint16)spell->getId()
	       << (sf::Uint8)(hideserverinfo ? NEUTRAL_PLAYER : spell->getOwner())
	       << (sf::Uint16)spell->getAppearance();
}

void RoundState::setupNewEntity(GameEntity *entity)
{
	entity->setFullySimulated(m_simulator.m_fullsimulator);
	entity->setInterpolationTime(m_simulator.m_interpolationtime);
	//Add it to the collision manager
	entity->setCollisionManager(m_colmgr.get());
	m_simulator.onEntityAdded(entity);
}
