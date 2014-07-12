#include "GameEntity.hpp"
#include <cassert>

GameEntity::GameEntity(RoundState &round, sf::Uint16 id):
	m_colobj(CollisionEntityType::Entity, this, 0.f),
	m_fullysimulated(true),
	m_round(round),
	m_id(id),
	m_owner(NEUTRAL_PLAYER),
	m_lastsnapshot(10)//To prevent interpolation when the game starts
{
	assert(id != NO_ENTITY_ID);
}

GameEntity::~GameEntity()
{

}
