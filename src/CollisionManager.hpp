#ifndef COLLISIONMANAGER_HPP_INCLUDED
#define COLLISIONMANAGER_HPP_INCLUDED

#include "CollisionObject.hpp"
#include "Map.hpp"
#include <unordered_set>
#include <list>

/*
Only add objects that cannot overlap (e.g. walls, characters...).
Do not add objects that will only be tested for collisions (e.g. non blocking spells etc).
For blocking spells ("barrier" spells), add it and enable overlap (so that they can still collide with each other).
*/

class CollisionManager
{
	public:
	CollisionManager(const Map &map);
	virtual ~CollisionManager();

	CollisionManager(const CollisionManager &) = delete;
	CollisionManager &operator=(const CollisionManager &) = delete;

	void attach(CollisionObject *object);
	void detach(CollisionObject *object);

	virtual void update(float etime) = 0;
	virtual void getObjectsVisibleFrom(unsigned int x, unsigned int y, std::list<CollisionObject *> &objects) const = 0;//objects should be empty

	protected:
	void notifyCollision(CollisionObject *a, CollisionObject *b);
	void addCorrection(CollisionObject *obj, const sf::Vector2f &correction);
	void applyCorrection(CollisionObject *obj, float correctionfactor);
	const Map &m_map;
	std::unordered_set<CollisionObject *> m_objects;
};

#endif // COLLISIONMANAGER_HPP_INCLUDED
