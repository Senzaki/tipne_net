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

constexpr float COLLISION_STEP_TIME = 1.f / 200.f;

class CollisionManager
{
	public:
	CollisionManager(const Map &map);
	virtual ~CollisionManager();

	CollisionManager(const CollisionManager &) = delete;
	CollisionManager &operator=(const CollisionManager &) = delete;

	void attach(CollisionObject *object);
	void detach(CollisionObject *object);

	void setPostStepCallBack(std::function<void()> callback);

	void update(float etime);
	virtual void getObjectsVisibleFrom(unsigned int x, unsigned int y, std::list<CollisionObject *> &objects) = 0;//objects should be empty

	protected:
	virtual void simulateStep() = 0;
	virtual void onObjectRemoved(CollisionObject *object) { }

	void notifyCollision(CollisionObject *a, CollisionObject *b);
	void addCorrection(CollisionObject *obj, const sf::Vector2f &correction);
	void applyCorrection(CollisionObject *obj, float correctionfactor);

	const Map &m_map;
	std::unordered_set<CollisionObject *> m_objects;
	float m_remainingtime;
	std::function<void()> m_poststep;
};

#endif // COLLISIONMANAGER_HPP_INCLUDED
