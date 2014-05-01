#include "DrawableMap.hpp"
#include <cassert>
#include "BasisChange.hpp"

const unsigned int CHUNK_SIZE = 16;

DrawableMap::DrawableMap()
{

}

void DrawableMap::update(float etime, const sf::FloatRect &seen)
{

}

void DrawableMap::draw(sf::RenderWindow &window, const sf::FloatRect &seen, std::list<DrawableEntity *> &mapentities)
{
	if(m_chunkscount.x == 0 || m_chunkscount.y == 0)
		return;
	//Translate the "seen" rectangle into grid coordinates then in chunks coords (we only need top, left, right and bottom of the bounding box)
	const unsigned int left = std::max(std::floor(BasisChange::pixelToGridX(seen.left, seen.top + seen.height) + 0.5f), 0.f);
	const unsigned int cleft = left / CHUNK_SIZE;
	const unsigned int top = std::max(std::floor(BasisChange::pixelToGridY(seen.left, seen.top) + 0.5f), 0.f);
	const unsigned int ctop = top / CHUNK_SIZE;
	const unsigned int right = std::max(std::floor(BasisChange::pixelToGridX(seen.left + seen.width, seen.top) + 0.5f), 0.f);
	const unsigned int cright = std::min(right / CHUNK_SIZE + 1, m_chunkscount.x);
	const unsigned int bottom = std::max(std::floor(BasisChange::pixelToGridY(seen.left + seen.width, seen.top + seen.height) + 0.5f), 0.f);
	const unsigned int cbottom = std::min(bottom / CHUNK_SIZE + 1, m_chunkscount.y);
	//Simply use the computed bounding box (unnecessary chunks will be drawn, but it is a very simple method)
	//We need to draw for x decreasing and y increasing (so that the depth decreases)
	for(unsigned int j = ctop; j < cbottom; j++)
	{
		for(unsigned int i = cright - 1; i != cleft - 1; i--)
		{
			m_chunks[j * m_chunkscount.x + i].draw(window);
			std::list<DrawableWall> &walls = m_chunks[j * m_chunkscount.x + i].getWalls();
			for(DrawableWall &character : walls)
				mapentities.emplace_back(&character);
		}
	}
}

bool DrawableMap::setMap(const Map &map)
{
	assert(map);
	//Compute the number of chunks
	const unsigned int maxi = (map.getSize().x - 1) / CHUNK_SIZE + 1;
	const unsigned int maxj = (map.getSize().y - 1) / CHUNK_SIZE + 1;
	m_chunks.resize(maxi * maxj);

	//Rect to tell each chunk its position & size
	sf::Rect<unsigned int> rect(0, 0, CHUNK_SIZE, CHUNK_SIZE);
	for(unsigned int i = 0; i < maxi; i++)
	{
		for(unsigned int j = 0; j < maxj; j++)
		{
			//Create a chunk from its associated groop of tiles (doesn't matter if the rect goes out of the map, the loadTiles function will ignore the tiles that don't exist)
			if(!m_chunks[j * maxi + i].loadTiles(map, rect))
			{
				m_chunks.clear();
				return false;
			}
			//Move the rect of the chunk (= go to the new chunk)
			rect.top += CHUNK_SIZE;
		}
		rect.top = 0;
		rect.left += CHUNK_SIZE;
	}

	//Set the size
	m_chunkscount.x = maxi;
	m_chunkscount.y = maxj;

	return true;
}
