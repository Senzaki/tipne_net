#include "make_unique.hpp"
#include <iostream>
#include <cassert>

template<typename EntityType, typename... Args>
EntityType *RoundState::addEntity(Args &&...args)
{
	auto entity = make_unique<EntityType>(std::forward<Args>(args)...);
	EntityType *addedent = entity.get();
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
	setupNewEntity(addedent);
#ifndef NDEBUG
	std::cout << "[DEBUG]New Entity. Id : " << (int)id << "." << std::endl;
#endif
	return addedent;
}
