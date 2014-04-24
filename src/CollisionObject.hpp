#ifndef COLLISIONOBJECT_HPP_INCLUDED
#define COLLISIONOBJECT_HPP_INCLUDED

#include <SFML/System.hpp>

class CollisionManager;

constexpr const float CLIENT_INTERPOLATION_TIME = 0.1f;

enum class CollisionEntityType
{
	None, //nullptr
	Bound, //nullptr
	Wall, //Cast to sf::Vector2u * (Representing the position. NOTE : any collision object of this type is very likely to be temporary.)
	Character //Cast to Character *
};

class CollisionObject
{
	public:
	CollisionObject(CollisionEntityType enttype, void *entity, float interpolationtime = 0.f);
	CollisionObject(CollisionObject &&other);
	~CollisionObject();

	CollisionObject(const CollisionObject &) = delete;
	CollisionObject &operator=(const CollisionObject &) = delete;

	void setPosition(float x, float y);//Sets "wanted" position, but it may not be the position returned by getPosition (e.g. interpolation)
	void setPosition(const sf::Vector2f &position);
	void forcePosition(float x, float y);//This one will force the position (no interpolation possible)
	void forcePosition(const sf::Vector2f &position);
	sf::Vector2f getPosition() const;
	sf::Vector2f getDesiredPosition() const;
	void setHalfSize(const sf::Vector2f &halfsize);//If the object becomes bigger, you may want to call CollisionManager::getNearestPosition to avoid collision bugs
	sf::Vector2f getHalfSize() const;

	CollisionEntityType getEntityType() const;
	void setEntity(void *entity);
	void *getEntity();
	bool isOverlappingAllowed() const;
	void setOverlappingAllowed(bool allow);

	CollisionManager *getCollisionManager();

	void setInterpolationTime(float interpolationtime);
	float getInterpolationTime() const;

	bool isStatic() const;

	bool update(float etime);//Returns true if the position has changed

	private:
	void updateTiles();

	CollisionEntityType m_enttype;
	void *m_entity;
	bool m_overlap;

	sf::Vector2f m_start;
	sf::Vector2f m_direction;
	sf::Vector2f m_position;
	sf::Vector2f m_desired;
	float m_maxtime;
	float m_time;
	bool m_justset;

	sf::Vector2f m_halfsize;

	CollisionManager *m_colmgr;//Set by attach/detach. Never modify it in any other way
	//Managed by the CollisionManager
	struct TileBounds
	{
		unsigned int minx;
		unsigned int maxx;
		unsigned int miny;
		unsigned int maxy;
	} m_tiles;

	friend class CollisionManager;
};

#endif // COLLISIONOBJECT_HPP_INCLUDED
