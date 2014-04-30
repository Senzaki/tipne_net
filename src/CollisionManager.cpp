#include "CollisionManager.hpp"
#include <cassert>

CollisionManager::CollisionManager(const Map &map):
	m_map(map)
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
	m_objects.erase(object);
	object->m_colmgr = nullptr;
}

void CollisionManager::notifyCollision(CollisionObject *a, CollisionObject *b)
{
	if(a->m_callback)
		a->m_callback(b);
	if(b->m_callback)
		b->m_callback(a);
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
