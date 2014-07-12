#include "LineDamageSpell.hpp"

LineDamageSpell::LineDamageSpell(RoundState &round, sf::Uint16 id, sf::Uint16 appearance, Character *caster, float radius, sf::Uint8 damage, bool destroyonhit, bool ignorewalls):
	SpellProjectile(round, id, appearance),
	LineSpellProjectile(round),
	DamageSpell(round, caster, damage, destroyonhit, ignorewalls)
{
	m_colobj.setRadius(radius);
}

LineDamageSpell::~LineDamageSpell()
{

}
