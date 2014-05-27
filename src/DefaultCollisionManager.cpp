#include "DefaultCollisionManager.hpp"
#include <cfloat>
#include <cmath>
#include <algorithm>
#include <iostream>

static const float CORRECTION_FACTOR = 0.8f;

static inline bool getCircleInTilePenetration(sf::Vector2f position, float radius, float x, float y, sf::Vector2f &p)
{
	//Find closest point in rectangle
	float closestx = position.x;
	if(closestx < x)
		closestx = x;
	else if(closestx > x + 1.f)
		closestx = x + 1.f;
	float closesty = position.y;
	if(closesty < y)
		closesty = y;
	else if(closesty > y + 1.f)
		closesty = y + 1.f;
	//Is this point in the circle ?
	float dx = closestx - position.x;
	float dy = closesty - position.y;
	if(dx == 0.f && dy == 0.f)
	{
		//Avoid div by 0, choose the penetration vector
		p = sf::Vector2f(radius, 0.f);
		return true;
	}
	if(dx * dx + dy * dy <= radius * radius)
	{
		//Yes, return with penetration vector set
		float dist = std::sqrt(dx * dx + dy * dy);
		p = sf::Vector2f(dx, dy) / dist * (dist - radius);
		return true;
	}
	return false;
}

DefaultCollisionManager::DefaultCollisionManager(const Map &map):
	CollisionManager(map),
	m_mapwidth(map.getSize().x),
	m_tilescontent(map.getSize().x * map.getSize().y + 4), //Last cell = out of bounds
	m_boundsobject(CollisionEntityType::Bound, nullptr),
	m_wallobject(CollisionEntityType::Wall, nullptr),
	m_needsrehash(true)
{
	for(unsigned int i = 0; i < 4; i++)
		m_boundsindex[i] = m_tilescontent.size() - 1 - i;
}

DefaultCollisionManager::~DefaultCollisionManager()
{

}

void DefaultCollisionManager::simulateStep()
{
	//Clear hash
	for(std::list<std::pair<CollisionObject *, sf::Vector2f>> &cell : m_tilescontent)
		cell.clear();
	m_needsrehash = false;
	//Move the objects and put them in the hash
	for(CollisionObject *object : m_objects)
	{
		object->updatePosition(COLLISION_STEP_TIME);
		rehashObject(object);
	}
	handleCollisions();
}

void DefaultCollisionManager::rehashObject(CollisionObject *object)
{
	//Compute bounds
	const sf::Vector2f position = object->getPosition();
	const float radius = object->getRadius();
	const float left = position.x - radius;
	const float right = position.x + radius;
	const float top = position.y - radius;
	const float bottom = position.y + radius;
	//Compute tiles bounds
	unsigned int minx;
	unsigned int maxx;
	unsigned int miny;
	unsigned int maxy;
	//Out of bounds left ?
	if(left < 0.f)
	{
		m_tilescontent[m_boundsindex[Left]].emplace_back(object, sf::Vector2f(-left, 0.f));
		minx = 0;
	}
	else
		minx = static_cast<unsigned int>(left);
	//Out of bounds right ?
	if(right >= static_cast<float>(m_map.getSize().x))
	{
		m_tilescontent[m_boundsindex[Right]].emplace_back(object, sf::Vector2f(static_cast<float>(m_map.getSize().x) - right, 0.f));
		maxx = m_map.getSize().x - 1;
	}
	else
		maxx = static_cast<unsigned int>(right);
	//Out of bounds top ?
	if(top < 0.f)
	{
		m_tilescontent[m_boundsindex[Top]].emplace_back(object, sf::Vector2f(0.f, -top));
		miny = 0;
	}
	else
		miny = static_cast<unsigned int>(top);
	//Out of bounds bottom ?
	if(bottom >= static_cast<float>(m_map.getSize().y))
	{
		m_tilescontent[m_boundsindex[Bottom]].emplace_back(object, sf::Vector2f(0.f, static_cast<float>(m_map.getSize().y) - bottom));
		maxy = m_map.getSize().y - 1;
	}
	else
		maxy = static_cast<unsigned int>(bottom);
	//Totally out of bounds ? (Left-Right of Top-Bottom)
	if(left >= static_cast<float>(m_map.getSize().x) || right < 0.f || top >= static_cast<float>(m_map.getSize().y) || bottom < 0.f)
		return;
	//Put the objects into the correct tiles
	sf::Vector2f p;
	for(unsigned int i = minx; i <= maxx; i++)
	{
		for(unsigned int j = miny; j <= maxy; j++)
		{
			if(getCircleInTilePenetration(position, radius, i, j, p))
				m_tilescontent[j * m_mapwidth + i].emplace_back(object, p);
		}
	}
}

void DefaultCollisionManager::getObjectsVisibleFrom(unsigned int x, unsigned int y, std::list<CollisionObject *> &objects)
{
	//Rehash if needed
	if(m_needsrehash)
	{
		for(std::list<std::pair<CollisionObject *, sf::Vector2f>> &cell : m_tilescontent)
			cell.clear();
		for(CollisionObject *object : m_objects)
			rehashObject(object);
		m_needsrehash = false;
	}
	//Add all the objects in visible tiles
	const std::vector<sf::Vector2u> &visibletiles = m_map.getTilesVisibleFrom(x, y);
	for(const sf::Vector2u &tilecoords : visibletiles)
	{
		//Do not add object from light-blocking tiles
		if(!Map::isAppearanceLightBlocking(m_map.getTile(tilecoords.x, tilecoords.y).appearance))
		{
			for(const std::pair<CollisionObject *, sf::Vector2f> &object : m_tilescontent[tilecoords.y * m_mapwidth + tilecoords.x])
				objects.emplace_back(object.first);
		}
	}
	std::unique(objects.begin(), objects.end());
}

void DefaultCollisionManager::onObjectRemoved(CollisionObject *object)
{
	m_needsrehash = true;
}

void DefaultCollisionManager::handleCollisions()
{
	//Check for collisions WITH each tile
	const unsigned int tilescount = m_tilescontent.size() - 4;
	for(unsigned int t = 0; t < tilescount; t++)
	{
		if(!m_map.getTileByHash(t).passable)
		{
			for(std::pair<CollisionObject *, sf::Vector2f> &objinfo : m_tilescontent[t])
			{
				CollisionObject *object = objinfo.first;
				notifyCollision(object, &m_wallobject);
				//Modify correction vector
				if(!object->isSensor())
					addCorrection(object, objinfo.second);
			}
		}
	}
	//Check for collisions with bounds
	for(unsigned t = tilescount; t < m_tilescontent.size(); t++)
	{
		for(std::pair<CollisionObject *, sf::Vector2f> &objinfo : m_tilescontent[t])
		{
			CollisionObject *object = objinfo.first;
			notifyCollision(object, &m_boundsobject);
			//Modify correction vector
			if(!object->isSensor())
				addCorrection(object, objinfo.second);
		}
	}
	//Check for collisions IN each tile (+ out of bounds)
	float dx;
	float dy;
	float sr;
	float d;
	sf::Vector2f p;
	for(unsigned int t = 0; t < m_tilescontent.size(); t++)
	{
		auto &tile = m_tilescontent[t];
		for(auto it1 = tile.begin(); it1 != tile.end(); it1++)
		{
			CollisionObject *obj1 = it1->first;
			for(auto it2 = std::next(it1); it2 != tile.end(); it2++)
			{
				CollisionObject *obj2 = it2->first;
				//Collision ?
				dx = obj1->getPosition().x - obj2->getPosition().x;
				dy = obj1->getPosition().y - obj2->getPosition().y;
				sr = obj1->getRadius() + obj2->getRadius();
				if(dx * dx + dy * dy < sr * sr)
				{
					notifyCollision(obj1, obj2);
					//Modify correction vector
					if(!(obj1->isSensor() || obj2->isSensor()))
					{
						if(dx == 0.f && dy == 0.f)
						{
							//Choose penetration vector to avoid div by 0
							p.x = std::min(obj1->getRadius(), obj2->getRadius());
							p.y = 0.f;
						}
						else
						{
							d = std::sqrt(dx * dx + dy * dy);
							//Compute penetration
							p.x = dx;
							p.y = dy;
							p *= (sr - d) / d;
						}
						addCorrection(obj1, p);
						addCorrection(obj2, -p);
					}
				}
			}
			//No other collisions for object 1, apply correction
			applyCorrection(obj1, CORRECTION_FACTOR);
		}
	}
}
