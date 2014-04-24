#include "CollisionObject.hpp"
#include "CollisionManager.hpp"
#include <cassert>

CollisionObject::CollisionObject(CollisionEntityType enttype, void *entity, float interpolationtime):
	m_enttype(enttype),
	m_entity(entity),
	m_overlap(false),
	m_maxtime(interpolationtime),
	m_time(0.f),
	m_justset(false),
	m_colmgr(nullptr)
{

}

CollisionObject::CollisionObject(CollisionObject &&other):
	m_enttype(other.m_enttype),
	m_entity(other.m_entity),
	m_overlap(other.m_overlap),
	m_start(other.m_start),
	m_direction(other.m_direction),
	m_position(other.m_position),
	m_desired(other.m_desired),
	m_maxtime(other.m_maxtime),
	m_time(other.m_time),
	m_justset(other.m_justset),
	m_halfsize(other.m_halfsize),
	m_colmgr(nullptr)
{
	other.m_entity = nullptr;
	//Swap the collision objects in the collision manager
	CollisionManager *mgr = other.m_colmgr;
	if(mgr)
	{
		mgr->detach(&other);
		mgr->attach(this);
	}
}

CollisionObject::~CollisionObject()
{
	if(m_colmgr)
		m_colmgr->detach(this);
}

void CollisionObject::setPosition(float x, float y)
{
#warning TEMP
	sf::Vector2f temp = m_desired;
	//Set the desired position & update the tile hash
	m_desired.x = x;
	m_desired.y = y;
	//Is it possible to go there ?
	if(m_colmgr)
	{
		if(m_colmgr->isColliding(this))
		{
			m_desired = temp;
			return;
		}
	}
	m_justset = true;
	updateTiles();
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
	m_justset = true;
	m_desired.x = x;
	m_desired.y = y;
	updateTiles();
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

void CollisionObject::setHalfSize(const sf::Vector2f &halfsize)
{
	assert(halfsize.x >= 0.f && halfsize.y >= 0.f);
	m_halfsize = halfsize;
	updateTiles();
}

sf::Vector2f CollisionObject::getHalfSize() const
{
	return m_halfsize;
}


CollisionEntityType CollisionObject::getEntityType() const
{
	return m_enttype;
}

void CollisionObject::setEntity(void *entity)
{
	m_entity = entity;
}

void *CollisionObject::getEntity()
{
	return m_entity;
}

bool CollisionObject::isOverlappingAllowed() const
{
	return m_overlap;
}

void CollisionObject::setOverlappingAllowed(bool allow)
{
	m_overlap = allow;
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

bool CollisionObject::update(float etime)
{
	if(isStatic())
	{
		//We need to notify a position change right after the position was forced
		if(m_justset)
		{
			m_justset = false;
			return true;
		}
		return false;
	}

	m_time += etime;
	m_position = m_start + m_direction * std::min(m_time / m_maxtime, 1.f);
	return true;
}

void CollisionObject::updateTiles()
{
	if(m_colmgr)
		m_colmgr->updateTilesForObject(this);
}
