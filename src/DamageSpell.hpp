#ifndef DAMAGESPELL_HPP_INCLUDED
#define DAMAGESPELL_HPP_INCLUDED

#include "SpellProjectile.hpp"

class DamageSpell : public virtual SpellProjectile
{
	public:
	DamageSpell(GameSimulator &simulator, Character *caster, sf::Uint8 damage, bool destroyonhit = true, bool ignorewalls = false);
	virtual ~DamageSpell();

	DamageSpell(const DamageSpell &) = delete;
	DamageSpell &operator=(const DamageSpell &) = delete;

	void setDamage(sf::Uint8 damage);
	sf::Uint8 getDamage() const;

	virtual void onCollision(CollisionObject *other);

	private:
	Character *m_caster;
	sf::Uint8 m_damage;
	bool m_ignorewalls;
	bool m_destroy;
	std::list<Character *> m_hit;
};

#endif // DAMAGESPELL_HPP_INCLUDED
