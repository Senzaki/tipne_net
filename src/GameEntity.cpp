#include "GameEntity.hpp"
#include <cassert>

GameEntity::GameEntity(GameSimulator &simulator, sf::Uint16 id, bool fullysimulated, float interpolationtime, sf::Uint8 owner, sf::Vector2f position):
	m_colobj(CollisionEntityType::Entity, this, interpolationtime),
	m_fullysimulated(fullysimulated),
	m_simulator(simulator),
	m_id(id),
	m_owner(owner),
	m_lastsnapshot(10)//To prevent interpolation when the game starts
{
	assert(id != NO_ENTITY_ID);
}

GameEntity::~GameEntity()
{

}
