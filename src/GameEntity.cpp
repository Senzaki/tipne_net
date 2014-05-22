#include "GameEntity.hpp"

GameEntity::GameEntity(sf::Uint16 id, bool fullysimulated, float interpolationtime, sf::Uint8 owner, sf::Vector2f position):
	m_colobj(CollisionEntityType::Entity, this, interpolationtime),
	m_fullysimulated(fullysimulated),
	m_simulator(nullptr),
	m_id(id),
	m_owner(owner),
	m_lastsnapshot(10)//To prevent interpolation when the game starts
{

}

GameEntity::~GameEntity()
{

}
