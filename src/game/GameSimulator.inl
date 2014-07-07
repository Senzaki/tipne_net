#include <iostream>
#include <cassert>
#include "make_unique.hpp"

template<typename... Args>
inline Player *GameSimulator::addPlayer(Args &&...args)
{
	return addPlayer(Player(std::forward<Args>(args)...));
}

template<typename EntityType, typename... Args>
EntityType *GameSimulator::addEntity(Args &&...args)
{
	auto entity = make_unique<EntityType>(std::forward<Args>(args)...);
	EntityType *addedent = entity.get();//It's ugly but there's no reason for it not to work...
	sf::Uint16 id = entity->getId();
	assert(id != NO_ENTITY_ID);
	auto added = m_entities.emplace(id, std::move(entity));
	if(!added.second)
	{
		//Entity id already exists
#ifndef NDEBUG
		std::cerr << "[DEBUG]Cannot create new entity. Id " << (int)id << " already reserved." << std::endl;
#endif
		return nullptr;
	}
	addedent->setFullySimulated(m_fullsimulator);
	addedent->setInterpolationTime(m_interpolationtime);
	//Add it to the collision manager
	addedent->setCollisionManager(m_colmgr.get());
	//Tell the listener if required
	if(m_statelistener)
		m_statelistener->onNewEntity(addedent);
#ifndef NDEBUG
	std::cout << "[DEBUG]New Entity. Id : " << (int)id << "." << std::endl;
#endif
	onEntityAdded(addedent);
	return addedent;
}
