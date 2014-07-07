#include "LineDamageSpell.hpp"

LineDamageSpell::LineDamageSpell(GameSimulator &simulator, sf::Uint16 id, sf::Uint16 appearance, Character *caster, float radius, sf::Uint8 damage, bool destroyonhit, bool ignorewalls):
	SpellProjectile(simulator, id, appearance),
	LineSpellProjectile(simulator),
	DamageSpell(simulator, caster, damage, destroyonhit, ignorewalls)
{
	m_colobj.setRadius(radius);
}

LineDamageSpell::~LineDamageSpell()
{

}
