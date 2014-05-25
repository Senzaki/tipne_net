#ifndef LINEDAMAGESPELL_HPP_INCLUDED
#define LINEDAMAGESPELL_HPP_INCLUDED

#include "LineSpellProjectile.hpp"
#include "DamageSpell.hpp"

class LineDamageSpell : public LineSpellProjectile, public DamageSpell
{
	public:
	LineDamageSpell(GameSimulator &simulator, sf::Uint16 id, sf::Uint16 appearance, Character *caster, float radius, sf::Uint8 damage, bool destroyonhit = true, bool ignorewalls = false);
	virtual ~LineDamageSpell();
};

#endif // LINEDAMAGESPELL_HPP_INCLUDED
