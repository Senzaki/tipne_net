#include "Character.hpp"
#include "CharacterStateListener.hpp"
#include "GameSimulator.hpp"
#include "CollisionManager.hpp"
#include <cmath>

static const float DEFAULT_SPEED = 3.f;

Character::~Character()
{

}

void Character::setCharacterStateListener(CharacterStateListener *listener)
{
	m_listener = listener;
	if(m_listener)
	{
		m_listener->onStateChanged(m_state);
		m_listener->onPositionChanged(m_colobj.getPosition());
	}
}

void Character::setState(State state)
{
	m_state = state;
	m_listener->onStateChanged(m_state);
}

Character::State Character::getState() const
{
	return m_state;
}

void Character::update(float etime)
{
	//Change the position if the character is moving (don't change it if it is not needed, because it would restart the interpolation time, thus preventing the character from really reaching its target)
	if(m_listener)
		m_listener->onPositionChanged(m_colobj.getPosition());
}

void Character::setDirection(sf::Vector2f direction)
{
	//Normalize the direction if non-zero
	if(!(direction.x == 0.f && direction.y == 0.f))
		direction = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
	if(m_listener)
		m_listener->onDirectionChanged(direction);
	if(m_direction != direction)
	{
		//Only tell the simulator if the direction has changed (to avoid sending extra packets)
		m_direction = direction;
		if(m_fullysimulated)
			m_colobj.setSpeed(m_direction * DEFAULT_SPEED);
	}
}

sf::Vector2f Character::getDirection() const
{
	return m_direction;
}
