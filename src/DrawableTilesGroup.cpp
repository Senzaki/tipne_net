#include "DrawableTilesGroup.hpp"
#include "ResourceManager.hpp"
#include <vector>
#include <list>
#include <iostream>
#include "BasisChange.hpp"

struct GraphTileInfo
{
	unsigned int texture;
	unsigned int framescount;
	struct
	{
		float x;
		float y;
		float width;
		float height;
		float center_x;
		float center_y;
		float time;
	} frames[16];
};

//Index is Tile::appearance
const unsigned int TILE_INFO_SIZE = 3;
GraphTileInfo TILE_INFO[TILE_INFO_SIZE] =
{
	//0
	{Resource::BASE_TILES, 1, {
		{0.f, 0.f, 100.f, 50.f, 50.f, 25.f}
	}},
	//1
	{Resource::BASE_TILES, 1, {
		{0.f, 50.f, 100.f, 50.f, 50.f, 25.f}
	}},
	//2
	{Resource::GRASS_TEST, 1, {
		{0.f, 0.f, 101.f, 51.f, 51.f, 26.f}
	}}
};

DrawableTilesGroup::DrawableTilesGroup()
{

}

bool DrawableTilesGroup::loadTiles(const Map &map, const sf::Rect<unsigned int> &rect)
{
	//Reduce the size of the rect to be sure it stays in the bounds
	float right = std::min(map.getSize().x, rect.left + rect.width);
	float bottom = std::min(map.getSize().y, rect.top + rect.height);
	std::vector<std::list<std::pair<const Tile *, sf::Vector2u>>> statictiles(Resource::MAP_TEXTURES_COUNT);
	unsigned int statictexcount = 0;
	//Iterate through the whole array to put the tiles into the hash table according to their texture
	for(unsigned int x = rect.left; x < right; x++)
	{
		for(unsigned int y = rect.top; y < bottom; y++)
		{
			const Tile *tile = &map.getTile(x, y);
			if(tile->appearance >= TILE_INFO_SIZE)
			{
				std::cerr << "Error while loading tiles : tile appearance invalid (" << tile->appearance << ")." << std::endl;
				return false;
			}
			if(TILE_INFO[tile->appearance].framescount == 1)
			{
				//If it's the first tile that uses this texture, increase the amount of textures to be used
				std::list<std::pair<const Tile *, sf::Vector2u>> &curtilelist = statictiles[TILE_INFO[tile->appearance].texture];
				if(curtilelist.size() == 0)
					statictexcount++;
				curtilelist.emplace_back(tile, sf::Vector2u(x, y));
			}
		}
	}

	m_statictiles.clear();
	m_statictiles.resize(statictexcount);

	unsigned int index = 0;
	for(unsigned int i = 0; index < statictexcount; i++)
	{
		if(statictiles[i].size() != 0)
		{
			//Set the texture
			m_statictiles[index].texture = &ResourceManager::getInstance().getTexture(ResourceSection::Map, TILE_INFO[statictiles[i].front().first->appearance].texture);
			//Resize the vertex array
			sf::VertexArray &va = m_statictiles[index].vertices;
			va.setPrimitiveType(sf::Quads);
			va.resize(statictiles[i].size() * 4);
			//Create info about the vertex
			unsigned int j = 0;
			for(const std::pair<const Tile *, sf::Vector2u> &tile : statictiles[i])
			{
				const GraphTileInfo &tileinfo = TILE_INFO[tile.first->appearance];
				const auto &frame = tileinfo.frames[0];
				//Center transformation (matrix for the change of basis)
				//center.x = 50 * (x + y)
				//center.y = 25 * (y - x)
				const float left = BasisChange::gridToPixelX(tile.second) - frame.center_x;//center.x - centeroffset.x
				const float top = BasisChange::gridToPixelY(tile.second) - frame.center_y;
				//top left hand vertex
				va[j].position.x = left;
				va[j].position.y = top;
				va[j].texCoords.x = frame.x;
				va[j].texCoords.y = frame.y;
				j++;
				//top right hand vertex
				va[j].position.x = left + frame.width;
				va[j].position.y = top;
				va[j].texCoords.x = frame.x + frame.width;
				va[j].texCoords.y = frame.y;
				j++;
				//bottom right hand vertex
				va[j].position.x = left + frame.width;
				va[j].position.y = top + frame.height;
				va[j].texCoords.x = frame.x + frame.width;
				va[j].texCoords.y = frame.y + frame.height;
				j++;
				//bottom left hand vertex
				va[j].position.x = left;
				va[j].position.y = top + frame.height;
				va[j].texCoords.x = frame.x;
				va[j].texCoords.y = frame.y + frame.height;
				j++;
			}
			index++;
		}
	}
	return true;
}

void DrawableTilesGroup::update(float etime)
{

}

void DrawableTilesGroup::draw(sf::RenderWindow &window)
{
	for(unsigned int i = 0; i < m_statictiles.size(); i++)
		window.draw(m_statictiles[i].vertices, m_statictiles[i].texture);
}
