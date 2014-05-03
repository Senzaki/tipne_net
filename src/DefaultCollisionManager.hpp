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

	virtual void update(float etime);
	virtual void getObjectsVisibleFrom(unsigned int x, unsigned int y, std::list<CollisionObject *> &objects);

	private:
	void updateObject(CollisionObject *object);//Returns true when out of bounds
	void handleCollisions();

	float m_remainingtime;

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
};

#endif // DEFAULTCOLLISIONMANAGER_HPP_INCLUDED
