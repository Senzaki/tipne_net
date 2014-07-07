#ifndef DEFAULTCOLLISIONMANAGER_HPP_INCLUDED
#define DEFAULTCOLLISIONMANAGER_HPP_INCLUDED

#include "CollisionManager.hpp"
#include <list>

class DefaultCollisionManager : public CollisionManager
{
	public:
	DefaultCollisionManager(const Map &map);
	virtual ~DefaultCollisionManager();

	DefaultCollisionManager(const DefaultCollisionManager &) = delete;
	DefaultCollisionManager &operator=(const DefaultCollisionManager &) = delete;

	virtual void getObjectsVisibleFrom(unsigned int x, unsigned int y, std::list<CollisionObject *> &objects);

	protected:
	virtual void simulateStep();
	virtual void onObjectRemoved(CollisionObject *object);

	private:
	void rehashObject(CollisionObject *object);
	void handleCollisions();

	unsigned int m_mapwidth;
	enum
	{
		Left,
		Right,
		Top,
		Bottom
	};
	unsigned int m_boundsindex[4];
	std::vector<std::list<std::pair<CollisionObject *, sf::Vector2f>>> m_tilescontent;
	CollisionObject m_boundsobject;
	CollisionObject m_wallobject;
	bool m_needsrehash;
};

#endif // DEFAULTCOLLISIONMANAGER_HPP_INCLUDED
