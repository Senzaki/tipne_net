#include "CollisionManager.hpp"
#include "GameEntity.hpp"
#include <cassert>

CollisionManager::CollisionManager(const Map &map):
	m_map(map),
	m_remainingtime(0.f)
{

}

CollisionManager::~CollisionManager()
{
	//Detach all the objects
	while(!m_objects.empty())
		detach(*m_objects.begin());
}

void CollisionManager::attach(CollisionObject *object)
{
	//Already used ?
	assert(object->m_colmgr != this);
	if(object->m_colmgr != nullptr)
		object->m_colmgr->detach(object);
	//Add it to the set and put it in the tiles hash
	m_objects.emplace(object);
	object->m_colmgr = this;
	//Reset correction vector
	object->m_correctionvect.x = 0.f;
	object->m_correctionvect.y = 0.f;
}

void CollisionManager::detach(CollisionObject *object)
{
	assert(object->m_colmgr == this);
	onObjectRemoved(object);
	m_objects.erase(object);
	object->m_colmgr = nullptr;
}

void CollisionManager::setPostStepCallBack(std::function<void()> callback)
{
	m_poststep = callback;
}

void CollisionManager::update(float etime)
{
	m_remainingtime += etime;
	while(m_remainingtime > COLLISION_STEP_TIME)
	{
		simulateStep();
		if(m_poststep)
			m_poststep();
		m_remainingtime -= COLLISION_STEP_TIME;
	}
}

void CollisionManager::notifyCollision(CollisionObject *a, CollisionObject *b)
{
	if(a->m_entity)
		a->m_entity->onCollision(b);
	if(b->m_entity)
		b->m_entity->onCollision(a);
}

void CollisionManager::addCorrection(CollisionObject *obj, const sf::Vector2f &correction)
{
	obj->m_correctionvect += correction;
}

void CollisionManager::applyCorrection(CollisionObject *obj, float correctionfactor)
{
	obj->correctPosition(obj->getPosition() + obj->m_correctionvect * correctionfactor);
	obj->m_correctionvect.x = 0.f;
	obj->m_correctionvect.y = 0.f;
}
