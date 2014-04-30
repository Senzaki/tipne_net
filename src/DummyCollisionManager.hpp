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
};

#endif // DUMMYCOLLISIONMANAGER_HPP_INCLUDED
