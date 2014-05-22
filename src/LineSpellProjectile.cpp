#include "LineSpellProjectile.hpp"

LineSpellProjectile::LineSpellProjectile(GameSimulator &simulator, sf::Vector2f speed):
	GameEntity(simulator, NO_ENTITY_ID),//Will never be called
	m_speed(speed)
{

}

LineSpellProjectile::~LineSpellProjectile()
{

}

void LineSpellProjectile::setSpeed(float x, float y)
{
	m_speed.x = x;
	m_speed.y = y;
}

const sf::Vector2f &LineSpellProjectile::getSpeed() const
{
	return m_speed;
}

void LineSpellProjectile::update(float etime)
{
	sf::Vector2f position = getPosition();
	position.x += m_speed.x * etime;
	position.y += m_speed.y * etime;
	setPosition(position.x, position.y);
}
