#include "SpellProjectile.hpp"

SpellProjectile::SpellProjectile(RoundState &round, sf::Uint16 id, sf::Uint16 appearance):
	GameEntity(round, id),
	m_listener(nullptr),
	m_appearance(appearance)
{

}

SpellProjectile::~SpellProjectile()
{

}

sf::Uint16 SpellProjectile::getAppearance() const
{
	return m_appearance;
}

void SpellProjectile::setStateListener(SpellProjectileStateListener *listener)
{
	m_listener = listener;
}
