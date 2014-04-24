#ifndef COLLISIONMANAGER_HPP_INCLUDED
#define COLLISIONMANAGER_HPP_INCLUDED

#include "CollisionObject.hpp"
#include "Map.hpp"
#include <list>
#include <unordered_set>

/*
Only add objects that cannot overlap (e.g. walls, characters...).
Do not add objects that will only be tested for collisions (e.g. non blocking spells etc).
For blocking spells ("barrier" spells), add it and enable overlap (so that they can still collide with each other).
*/

class CollisionManager
{
	public:
	CollisionManager(const Map &map);
	~CollisionManager();

	CollisionManager(const CollisionManager &) = delete;
	CollisionManager &operator=(const CollisionManager &) = delete;

	void setMap(const Map *map);

	void attach(CollisionObject *object);
	void detach(CollisionObject *object);

	bool isColliding(CollisionObject *object) const;//Objects with overlapping allowed ARE NOT considered as a collision.
	void foreachCollision(CollisionObject *object, const std::function<bool(CollisionObject *)> &callback);//Objects with overlapping allowed ARE considered as a collision.

	void updateTilesForObject(CollisionObject *object);

	private:
	void getTilesForObject(const sf::Vector2f &position, const sf::Vector2f &halfsize, CollisionObject::TileBounds &tiles);

	unsigned int m_mapwidth;

	const Map &m_map;
	std::vector<std::list<CollisionObject *>> m_tilescontent;
	std::unordered_set<CollisionObject *> m_objects;
	CollisionObject m_boundsobject;
};

#endif // COLLISIONMANAGER_HPP_INCLUDED
