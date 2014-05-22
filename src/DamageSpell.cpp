#include "DamageSpell.hpp"
#include "GameSimulator.hpp"

DamageSpell::DamageSpell(GameSimulator &simulator, unsigned int damage, bool destroyonhit, bool ignorewalls):
	GameEntity(simulator, NO_ENTITY_ID),//Will never be called
	m_damage(damage),
	m_ignorewalls(ignorewalls),
	m_destroy(destroyonhit)
{

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
				if(m_destroy || std::find(m_hit.begin(), m_hit.end(), character) == m_hit.end())
				{
					//TODO: Deal damage
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
