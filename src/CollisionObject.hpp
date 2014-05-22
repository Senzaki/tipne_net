#ifndef COLLISIONOBJECT_HPP_INCLUDED
#define COLLISIONOBJECT_HPP_INCLUDED

#include <functional>
#include <SFML/System.hpp>

class CollisionManager;
class GameEntity;

constexpr const float CLIENT_INTERPOLATION_TIME = 0.1f;

enum class CollisionEntityType
{
	None, //entity = nullptr
	Bound, //entity = nullptr
	Wall, //entity = nullptr
	Entity
};

class CollisionObject
{
	public:
	CollisionObject(CollisionEntityType enttype, GameEntity *entity, float interpolationtime = 0.f);
	~CollisionObject();

	CollisionObject(const CollisionObject &) = delete;
	CollisionObject &operator=(const CollisionObject &) = delete;

	void setPosition(float x, float y);//Sets "wanted" position, but it may not be the position returned by getPosition (e.g. interpolation)
	void setPosition(const sf::Vector2f &position);
	void forcePosition(float x, float y);//This one will force the position (no interpolation possible)
	void forcePosition(const sf::Vector2f &position);
	sf::Vector2f getPosition() const;
	sf::Vector2f getDesiredPosition() const;
	void setRadius(float radius);//If the object becomes bigger, you may want to call CollisionManager::getNearestPosition to avoid collision bugs
	float getRadius() const;
	void setSpeed(const sf::Vector2f &speed);
	sf::Vector2f getSpeed() const;

	CollisionEntityType getEntityType() const;
	void setEntity(GameEntity *entity);
	GameEntity *getEntity();
	bool isSensor() const;
	void setSensor(bool sensor);

	CollisionManager *getCollisionManager();

	void setInterpolationTime(float interpolationtime);
	float getInterpolationTime() const;

	bool isStatic() const;

	void updatePosition(float etime);//Returns true if the position has changed
	void correctPosition(const sf::Vector2f &position);

	private:
	CollisionEntityType m_enttype;
	GameEntity *m_entity;
	bool m_sensor;

	sf::Vector2f m_start;
	sf::Vector2f m_direction;
	sf::Vector2f m_position;
	sf::Vector2f m_desired;
	float m_maxtime;
	float m_time;
	sf::Vector2f m_speed;

	float m_radius;

	CollisionManager *m_colmgr;//Set by attach/detach. Never modify it in any other way
	sf::Vector2f m_correctionvect;//Set by the CollisionManager. Never modify it in any other way

	friend class CollisionManager;
};

#endif // COLLISIONOBJECT_HPP_INCLUDED
