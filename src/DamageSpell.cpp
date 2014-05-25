#include "DamageSpell.hpp"
#include "GameSimulator.hpp"

DamageSpell::DamageSpell(GameSimulator &simulator, Character *caster, sf::Uint8 damage, bool destroyonhit, bool ignorewalls):
	SpellProjectile(simulator, NO_ENTITY_ID, 0),//Will never be called
	m_caster(caster),
	m_damage(damage),
	m_ignorewalls(ignorewalls),
	m_destroy(destroyonhit)
{
	m_colobj.setSensor(true);
}

DamageSpell::~DamageSpell()
{

}

void DamageSpell::setDamage(sf::Uint8 damage)
{
	m_damage = damage;
}

sf::Uint8 DamageSpell::getDamage() const
{
	return m_damage;
}

void DamageSpell::onCollision(CollisionObject *other)
{
	if(m_fullysimulated)
	{
		switch(other->getEntityType())
		{
			case CollisionEntityType::Bound:
				m_simulator.removeEntityLater(getId());
				break;

			case CollisionEntityType::Wall:
				if(!m_ignorewalls)
					m_simulator.removeEntityLater(getId());
				break;

			case CollisionEntityType::Entity:
				if(Character *character = dynamic_cast<Character *>(other->getEntity()))
				{
					if(character == m_caster)
						return;
					if(m_destroy || std::find(m_hit.begin(), m_hit.end(), character) == m_hit.end())
					{
						//TODO: Deal damage
						//TEMP
						character->forcePosition(0.f, 0.f);
						if(!m_destroy)
							m_hit.emplace_back(character);
					}
					if(m_destroy)
						m_simulator.removeEntityLater(getId());
				}
				break;

			default:
			break;
		}
	}
}
