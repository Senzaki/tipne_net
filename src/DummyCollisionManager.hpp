#ifndef DUMMYCOLLISIONMANAGER_HPP_INCLUDED
#define DUMMYCOLLISIONMANAGER_HPP_INCLUDED

#include "CollisionManager.hpp"

class DummyCollisionManager : public CollisionManager
{
	public:
	DummyCollisionManager(const Map &map);
	virtual ~DummyCollisionManager();

	DummyCollisionManager(const DummyCollisionManager &) = delete;
	DummyCollisionManager &operator=(const DummyCollisionManager &) = delete;

	virtual void update(float etime);
	virtual void getObjectsVisibleFrom(unsigned int x, unsigned int y, std::list<CollisionObject *> &objects);//Warning : very stupid : returns everything !
};

#endif // DUMMYCOLLISIONMANAGER_HPP_INCLUDED
