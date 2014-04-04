#include "PositionManager.hpp"

PositionManager::PositionManager(float interpolationtime):
	m_maxtime(interpolationtime),
	m_time(0.f),
	m_justset(false)
{

}

void PositionManager::setPosition(float x, float y)
{
	m_justset = true;
	m_desired.x = x;
	m_desired.y = y;
	if(m_maxtime != 0.f)
	{
		m_start = m_position;
		m_direction.x = x - m_start.x;
		m_direction.y = y - m_start.y;
		m_time = 0.f;
	}
	else
	{
		m_position.x = x;
		m_position.y = y;
	}
}

void PositionManager::setPosition(const sf::Vector2f &position)
{
	setPosition(position.x, position.y);
}

void PositionManager::forcePosition(float x, float y)
{
	m_justset = true;
	m_desired.x = x;
	m_desired.y = y;
	m_position.x = x;
	m_position.y = y;
	if(m_maxtime != 0.f)
	{
		m_start.x = x;
		m_start.y = y;
		m_direction.x = 0.f;
		m_direction.y = 0.f;
	}
}

void PositionManager::forcePosition(const sf::Vector2f &position)
{
	forcePosition(position.x, position.y);
}

sf::Vector2f PositionManager::getPosition() const
{
	return m_position;
}

sf::Vector2f PositionManager::getDesiredPosition() const
{
	return m_desired;
}

void PositionManager::setInterpolationTime(float interpolationtime)
{
	if(m_maxtime == 0.f && interpolationtime != 0.f)
	{
		m_start.x = m_position.x;
		m_start.y = m_position.y;
		m_direction.x = 0.f;
		m_direction.y = 0.f;
	}

	m_maxtime = interpolationtime;
}

float PositionManager::getInterpolationTime() const
{
	return m_maxtime;
}

bool PositionManager::isStatic() const
{
	return (m_time >= m_maxtime || (m_direction.x == 0.f && m_direction.y == 0.f));
}

bool PositionManager::update(float etime)
{
	if(isStatic())
	{
		//We need to notify a position changed right after the position was forced
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
