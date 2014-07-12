#include "LineSpellProjectile.hpp"
#include "SpellProjectileStateListener.hpp"
#include "GameSimulator.hpp"

LineSpellProjectile::LineSpellProjectile(RoundState &round, const sf::Vector2f &speed, const sf::Vector2f &startpos, float range):
	SpellProjectile(round, NO_ENTITY_ID, 0),//Will never be called
	m_start(startpos),
	m_rangesq(range * range)
{

}

LineSpellProjectile::~LineSpellProjectile()
{

}

void LineSpellProjectile::setSpeed(const sf::Vector2f &speed)
{
	m_colobj.setSpeed(speed);
}

sf::Vector2f LineSpellProjectile::getSpeed() const
{
	return m_colobj.getSpeed();
}

void LineSpellProjectile::update(float etime)
{
	if(m_listener)
		m_listener->onPositionChanged(m_colobj.getPosition());
	//If the distance is greater than the range, destroy the projectile
	if(m_rangesq != 0.f * 0.f)
	{
		sf::Vector2f position = m_colobj.getPosition();
		const float dx = position.x - m_start.x;
		const float dy = position.x - m_start.y;
		if(dx * dx + dy * dy >= m_rangesq)
			m_round.removeEntityLater(getId());
	}
}
