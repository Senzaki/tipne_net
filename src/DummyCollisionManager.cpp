#include "DummyCollisionManager.hpp"

DummyCollisionManager::DummyCollisionManager(const Map &map):
	CollisionManager(map)
{

}

DummyCollisionManager::~DummyCollisionManager()
{

}

void DummyCollisionManager::update(float etime)
{
	//Simply update all the positions
	for(CollisionObject *object : m_objects)
		object->updatePosition(etime);
}
