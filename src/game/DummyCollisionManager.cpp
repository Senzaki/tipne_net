#include "DummyCollisionManager.hpp"
#include <iostream>

DummyCollisionManager::DummyCollisionManager(const Map &map):
	CollisionManager(map)
{

}

DummyCollisionManager::~DummyCollisionManager()
{

}

void DummyCollisionManager::simulateStep()
{
	//Simply update all the positions
	for(CollisionObject *object : m_objects)
		object->updatePosition(COLLISION_STEP_TIME);
}

void DummyCollisionManager::getObjectsVisibleFrom(unsigned int x, unsigned int y, std::list<CollisionObject *> &objects)
{
#ifndef NDEBUG
	std::cerr << "[DEBUG]Warning : The visibility function of the dummy collision manager should never be used." << std::endl;
#endif
	for(CollisionObject *object : m_objects)
		objects.emplace_back(object);
}
