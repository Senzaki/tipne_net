#ifndef SPELLPROJECTILE_HPP_INCLUDED
#define SPELLPROJECTILE_HPP_INCLUDED

#include "GameEntity.hpp"
class SpellProjectileStateListener;

class SpellProjectile : public GameEntity
{
	public:
	SpellProjectile(RoundState &state, sf::Uint16 id, sf::Uint16 appearance);
	virtual ~SpellProjectile();

	sf::Uint16 getAppearance() const;
	void setStateListener(SpellProjectileStateListener *listener);

	protected:
	SpellProjectileStateListener *m_listener;

	private:
	sf::Uint16 m_appearance;
};

#endif // SPELLPROJECTILE_HPP_INCLUDED
