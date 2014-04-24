#include "CollisionManager.hpp"
#include <cassert>

CollisionManager::CollisionManager(const Map &map):
	m_map(map),
	m_tilescontent(map.getSize().x * map.getSize().y)
{

}

CollisionManager::~CollisionManager()
{
	//Detach all the objects
	while(!m_objects.empty())
		detach(*m_objects.begin());
}

void CollisionManager::attach(CollisionObject *object)
{
	//Already used ?
	assert(object->m_colmgr != this);
	if(object->m_colmgr != nullptr)
		object->m_colmgr->detach(object);
	//Add it to the set and put it in the tiles hash
	m_objects.emplace(object);
	object->m_colmgr = this;
	updateTilesForObject(object);
}

void CollisionManager::detach(CollisionObject *object)
{
	assert(object->m_colmgr == this);
	m_objects.erase(object);

	//Remove it from the hash tiles
	const unsigned int width = m_map.getSize().x;
	for(unsigned int i = object->m_tiles.minx; i <= object->m_tiles.maxx; i++)
	{
		for(unsigned int j = object->m_tiles.miny; j <= object->m_tiles.maxy; j++)
			m_tilescontent[j * width + i].remove(object);
	}

	object->m_colmgr = nullptr;
}

bool CollisionManager::isColliding(CollisionObject *object) const
{
	const float x = object->m_desired.x;
	const float y = object->m_desired.y;
	const float w = object->m_halfsize.x;
	const float h = object->m_halfsize.y;
	//Test collisions with each object on the same tiles
	const unsigned int width = m_map.getSize().x;
	for(unsigned int i = object->m_tiles.minx; i <= object->m_tiles.maxx; i++)
	{
		for(unsigned int j = object->m_tiles.miny; j <= object->m_tiles.maxy; j++)
		{
			const unsigned int t = j * width + i;
			//Not passable tile ?
			if(!m_map.getTileByHash(t).passable)
				return true;
			for(CollisionObject *totest : m_tilescontent[t])
			{
				if(totest == object)
					continue;
				//Simple AABB test
				if((std::abs(x - totest->m_desired.x) < (w + object->m_halfsize.x)) && (std::abs(y - totest->m_desired.y) < (h + object->m_halfsize.y)))
					return true;
			}
		}
	}
	return false;
}

void CollisionManager::foreachCollision(CollisionObject *object, const std::function<bool(CollisionObject *)> &callback)
{

}

sf::Vector2f CollisionManager::getNearestPosition(CollisionObject *object, const sf::Vector2f &desiredpos)
{

}

sf::Vector2f CollisionManager::getIntermediatePosition(CollisionObject *object, const sf::Vector2f &desiredpos)
{

}

void CollisionManager::updateTilesForObject(CollisionObject *object)
{
	const unsigned int width = m_map.getSize().x;
	//Remove object from former tiles
	if(object->m_colmgr == this)
	{
		for(unsigned int i = object->m_tiles.minx; i <= object->m_tiles.maxx; i++)
		{
			for(unsigned int j = object->m_tiles.miny; j <= object->m_tiles.maxy; j++)
				m_tilescontent[j * width + i].remove(object);
		}
	}
	//Compute new tiles
	getTilesForObject(object->m_desired, object->m_halfsize, object->m_tiles);
	//Add object to new tiles
	if(object->m_colmgr == this)
	{
		for(unsigned int i = object->m_tiles.minx; i <= object->m_tiles.maxx; i++)
		{
			for(unsigned int j = object->m_tiles.miny; j <= object->m_tiles.maxy; j++)
				m_tilescontent[j * width + i].emplace_back(object);
		}
	}
}

void CollisionManager::getTilesForObject(const sf::Vector2f &position, const sf::Vector2f &halfsize, CollisionObject::TileBounds &tiles)
{
	//Compute bounds
	tiles.minx = static_cast<unsigned int>(std::max(0.f, position.x - halfsize.x));
	tiles.maxx = std::min(m_map.getSize().x - 1, static_cast<unsigned int>(std::max(0.f, position.x + halfsize.x)) + 1);
	tiles.miny = static_cast<unsigned int>(std::max(0.f, position.y - halfsize.y));
	tiles.maxy = std::min(m_map.getSize().y - 1, static_cast<unsigned int>(std::max(0.f, position.y + halfsize.y)) + 1);
}
