#include "CollisionObject.hpp"
#include "CollisionManager.hpp"
#include <cassert>

CollisionObject::CollisionObject(CollisionEntityType enttype, GameEntity *entity, float interpolationtime):
	m_enttype(enttype),
	m_entity(entity),
	m_sensor(false),
	m_maxtime(interpolationtime),
	m_time(0.f),
	m_colmgr(nullptr)
{

}

CollisionObject::~CollisionObject()
{
	if(m_colmgr)
		m_colmgr->detach(this);
}

void CollisionObject::setPosition(float x, float y)
{
	//Set the desired position
	m_desired.x = x;
	m_desired.y = y;
	//Interpolation ?
	if(m_maxtime != 0.f)
	{
		//Prepare interpolation
		m_start = m_position;
		m_direction.x = x - m_start.x;
		m_direction.y = y - m_start.y;
		m_time = 0.f;
	}
	else
	{
		//Directly set the position
		m_position.x = x;
		m_position.y = y;
	}
}

void CollisionObject::setPosition(const sf::Vector2f &position)
{
	setPosition(position.x, position.y);
}

void CollisionObject::forcePosition(float x, float y)
{
	//Directly set the position & update the tile hash
	m_desired.x = x;
	m_desired.y = y;
	m_position.x = x;
	m_position.y = y;
	//Cancel current interpolation
	if(m_maxtime != 0.f)
	{
		m_start.x = x;
		m_start.y = y;
		m_direction.x = 0.f;
		m_direction.y = 0.f;
	}
}

void CollisionObject::forcePosition(const sf::Vector2f &position)
{
	forcePosition(position.x, position.y);
}

sf::Vector2f CollisionObject::getPosition() const
{
	return m_position;
}

sf::Vector2f CollisionObject::getDesiredPosition() const
{
	return m_desired;
}

void CollisionObject::setRadius(const float radius)
{
	m_radius = radius;
}

float CollisionObject::getRadius() const
{
	return m_radius;
}

void CollisionObject::setSpeed(const sf::Vector2f &speed)
{
	m_speed = speed;
}

sf::Vector2f CollisionObject::getSpeed() const
{
	return m_speed;
}

CollisionEntityType CollisionObject::getEntityType() const
{
	return m_enttype;
}

void CollisionObject::setEntity(GameEntity *entity)
{
	m_entity = entity;
}

GameEntity *CollisionObject::getEntity()
{
	return m_entity;
}

bool CollisionObject::isSensor() const
{
	return m_sensor;
}

void CollisionObject::setSensor(bool sensor)
{
	m_sensor = sensor;
}

CollisionManager *CollisionObject::getCollisionManager()
{
	return m_colmgr;
}

void CollisionObject::setInterpolationTime(float interpolationtime)
{
	if(m_maxtime == 0.f && interpolationtime != 0.f)
	{
		//Prepare interpolation
		m_start.x = m_position.x;
		m_start.y = m_position.y;
		m_direction.x = 0.f;
		m_direction.y = 0.f;
	}

	m_maxtime = interpolationtime;
}

float CollisionObject::getInterpolationTime() const
{
	return m_maxtime;
}

bool CollisionObject::isStatic() const
{
	return (m_time >= m_maxtime || (m_direction.x == 0.f && m_direction.y == 0.f));
}

void CollisionObject::updatePosition(float etime)
{
	//Change speed ?
	if(m_speed.x != 0.f || m_speed.y != 0.f)
		setPosition(m_desired + m_speed * etime);

	if(isStatic())
		return;

	//Interpolation
	m_time += etime;
	m_position = m_start + m_direction * std::min(m_time / m_maxtime, 1.f);
}

void CollisionObject::correctPosition(const sf::Vector2f &position)
{
	if(m_maxtime == 0.f || m_time >= m_maxtime)
		forcePosition(position.x, position.y);
	else
	{
		m_position.x = position.x;
		m_position.y = position.y;
		//Continue interpolation from a different start position (disabling it makes the client more accurate, but the displayed data is less stable)
		//Linear 2-eq system solution
		m_direction = (m_desired - m_position) / (1.f - m_time / m_maxtime);
		m_start = m_desired - m_direction;
	}
}
