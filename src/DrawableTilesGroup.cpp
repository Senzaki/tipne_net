#include "DrawableTilesGroup.hpp"
#include "ResourceManager.hpp"
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
	{Resource::BASE_TILES_TEX, 1, {
		{0.f, 0.f, 160.f, 80.f, 80.f, 40.f}
	}},
	//1
	{Resource::BASE_TILES_TEX, 1, {
		{0.f, 80.f, 160.f, 80.f, 80.f, 40.f}
	}},
	//2
	{Resource::GRASS_TEST_TEX, 1, {
		{0.f, 0.f, 162.f, 82.f, 81.f, 42.f}
	}}
};

struct TileSortInfo
{
	const Tile *tile;
	unsigned int tileset;
	sf::Vector2u pos;
	unsigned int texture;
};

DrawableTilesGroup::DrawableTilesGroup()
{

}

static void sortFurtherTiles(std::vector<TileSortInfo> &tiles, unsigned int curdepth, unsigned int depths, unsigned int height, unsigned int width, unsigned int curset, unsigned int texture, std::vector<std::list<std::pair<GraphTileInfo *, sf::Vector2u>>> &tilesets)
{
	//Check this depth for tiles we can add. The requirements are :
	//- The tile must use the same texture as the orther tiles of the tileset
	//- The two tiles below it must have already been added to a tileset
	const unsigned int xmin = (curdepth < height) ? 0 : curdepth - height + 1;
	const unsigned int xmax = std::min(curdepth + 1, width);
	bool neednext = false;//Boolean used to check that we added at least one tile on this depth, because if we didn't, there's no need to go to the next line
	//Iterate throuh the tiles of this depth
	for(unsigned int x = xmin; x < xmax; x++)
	{
		const unsigned int y = x + height - 1 - curdepth;
		TileSortInfo &sortinf = tiles[x + y * width];
		if(sortinf.texture == texture)
		{
			bool add = true;
			//Check the tiles below it
			if(x > 0)
			{
				if(tiles[(x - 1) + y * width].tileset == 0)
					add = false;
			}
			if(add && y < height - 1)
			{
				if(tiles[x + (y + 1) * width].tileset == 0)
					add = false;
			}
			if(add)
			{
				neednext = true;
				sortinf.tileset = curset;
				tilesets.back().emplace_back(std::make_pair(&TILE_INFO[sortinf.tile->appearance], sortinf.pos));
			}
		}
	}
	//Go to the next depth if we added at least one line
	if(neednext && curdepth + 1 < depths)
		sortFurtherTiles(tiles, curdepth + 1, depths, height, width, curset, texture, tilesets);
}

bool DrawableTilesGroup::loadTiles(const Map &map, const sf::Rect<unsigned int> &rect)
{
	const unsigned int right = std::min(map.getSize().x, rect.left + rect.width);
	const unsigned int bottom = std::min(map.getSize().y, rect.top + rect.height);
	const unsigned int width = right - rect.left;
	const unsigned int height = bottom - rect.top;
	const unsigned int depths = width + height - 1;
	//Fill an array with the tiles
	std::vector<TileSortInfo> tiles;
	tiles.reserve(width * height);
	for(unsigned int y = rect.top; y < bottom; y++)
	{
		for(unsigned int x = rect.left; x < right; x++)
		{
			const Tile *tile = &map.getTile(x, y);
			if(tile->appearance >= TILE_INFO_SIZE)
			{
				std::cerr << "Error while loading tiles : tile appearance invalid (" << tile->appearance << ")." << std::endl;
				return false;
			}
			tiles.push_back({tile, 0, sf::Vector2u(x, y), TILE_INFO[tile->appearance].texture});
		}
	}

	//Put the tiles into tilesets that correspond to their (reverse) drawing order (reserve an arbitrary size for the array, we can assume that the tilesets will contain at least two tiles on average)
	std::vector<std::list<std::pair<GraphTileInfo *, sf::Vector2u>>> sortedtiles;
	sortedtiles.reserve(width * height / 2);
	//Iterate through the whole array to put the tiles into the hash table according to their texture (depth by depth)
	unsigned int curset = 0;
	for(unsigned int d = 0; d < depths; d++)
	{
		const unsigned int xmin = (d < height) ? 0 : d - height + 1;
		const unsigned int xmax = std::min(d + 1, width);
		for(unsigned int x = xmin; x < xmax; x++)
		{
			const unsigned int y = x + height - 1 - d;
			TileSortInfo &sortinf = tiles[x + y * width];
			//If the tile isn't in a tileset yet, create a new tileset
			if(sortinf.tileset == 0)
			{
				curset++;
				//Create a tileset and add this tile to the tileset
				sortinf.tileset = curset;
				sortedtiles.emplace_back();
				sortedtiles.back().emplace_back(std::make_pair(&TILE_INFO[sortinf.tile->appearance], sortinf.pos));
				//Add all the tiles on the same depth with the same texture to this tileset
				for(unsigned int x2 = x + 1; x2 < xmax; x2++)
				{
					const unsigned int y2 = x2 + height - 1 - d;
					TileSortInfo &sortinf2 = tiles[x2 + y2 * width];
					if(sortinf2.tileset == 0 && sortinf2.texture == sortinf.texture)
					{
						sortinf2.tileset = curset;
						sortedtiles.back().emplace_back(std::make_pair(&TILE_INFO[sortinf2.tile->appearance], sortinf2.pos));
					}
				}
				//Add all the other possible tiles to this tileset
				if(d + 1 < depths)
					sortFurtherTiles(tiles, d + 1, depths, height, width, curset, sortinf.texture, sortedtiles);
			}
		}
	}

	m_tilesets.clear();
	m_tilesets.resize(sortedtiles.size());

	//Now, create a vertex array for each tileset
	for(unsigned int i = 0; i < m_tilesets.size(); i++)
	{
		//Note : since the tilesets were added to the array from bottom to top, we need to reverse this order so that they are drawn by order of depth
		//Set the texture
		m_tilesets[i].texture = &ResourceManager::getInstance().getTexture(ResourceSection::Map, sortedtiles[m_tilesets.size() - i - 1].front().first->texture);
		//Resize the vertex array
		sf::VertexArray &va = m_tilesets[i].vertices;
		va.setPrimitiveType(sf::Quads);
		va.resize(sortedtiles[m_tilesets.size() - i - 1].size() * 4);
		//Create info about the vertices
		unsigned int j = 0;
		for(const std::pair<GraphTileInfo *, sf::Vector2u> &tile : sortedtiles[m_tilesets.size() - i - 1])
		{
			const GraphTileInfo &tileinfo = *tile.first;
			const auto &frame = tileinfo.frames[0];
			//Center transformation (matrix for the change of basis)
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
	}

	return true;
}

void DrawableTilesGroup::update(float etime)
{

}

void DrawableTilesGroup::draw(sf::RenderWindow &window)
{
	for(unsigned int i = 0; i < m_tilesets.size(); i++)
		window.draw(m_tilesets[i].vertices, m_tilesets[i].texture);
}
