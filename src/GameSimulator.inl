#include <iostream>
#include <cassert>

template<typename... Args>
inline Player *GameSimulator::addPlayer(Args &&...args)
{
	return addPlayer(Player(std::forward<Args>(args)...));
}

template<typename EntityType, typename... Args>
EntityType *GameSimulator::addEntity(Args &&...args)
{
	EntityType *entity = new EntityType(std::forward<Args>(args)...);
	if(addEntity(entity))
		return entity;
	//Error, could not add entity
	delete entity;
	return nullptr;
}
