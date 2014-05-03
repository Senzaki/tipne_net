#include "Map.hpp"
#include "BinaryFile.hpp"
#include <iostream>

static const std::string MAP_PREFIX = "data/maps/";
static const std::string MAP_SUFFIX = ".map";

Map::Map():
	m_loaded(false)
{

}

Map::Map(Map &&other):
	m_loaded(false),
	m_name(std::move(other.m_name)),
	m_tiles(std::move(other.m_tiles)),
	m_visibilitymap(std::move(other.m_visibilitymap)),
	m_size(other.m_size)
{

}

Map::~Map()
{

}

Map &Map::operator=(Map &&other)
{
	m_name = std::move(other.m_name);
	m_tiles = std::move(other.m_tiles);
	m_size = other.m_size;
	return *this;
}

bool Map::load(const std::string &name)
{
	m_loaded = false;
	m_tiles.clear();
	m_visibilitymap.clear();
	const std::string filename = MAP_PREFIX + name + MAP_SUFFIX;
	//Open the map file
	BinaryFile file(filename.c_str(), std::ios::in);
	if(!file)
	{
		std::cout << "Cannot open map file " << filename << "." << std::endl;
		return false;
	}

	//Read the size of the map
	{
		sf::Uint16 w, h;
		if(!(file >> w >> h))
		{
			std::cerr << "Error while loading map file " << filename << " : Map size section incomplete." << std::endl;
			return false;
		}
		if(w == 0 || h == 0)
		{
			std::cerr << "Error while loading map file " << filename << " : Map size cannot be 0 !" << std::endl;
			return false;
		}
		m_size.x = w;
		m_size.y = h;
	}
	//Resize the tiles array
	const unsigned int tilescount = m_size.x * m_size.y;
	m_tiles.resize(tilescount);
	m_visibilitymap.resize(tilescount);
	//Read each tile
	{
		sf::Uint16 visibletilescount;
		sf::Uint16 x, y;
		for(unsigned int i = 0; i < tilescount; i++)
		{
			if(!(file >> m_tiles[i]))
			{
				std::cerr << "Error while loading map file " << filename << " : Tiles data section incomplete." << std::endl;
				return false;
			}
			//Read the visible tiles for this tile
			if(!(file >> visibletilescount))
			{
				std::cerr << "Error while loading map file " << filename << " : Visibility map data section incomplete." << std::endl;
				return false;
			}
			m_visibilitymap[i].resize(visibletilescount);
			for(unsigned int j = 0; j < visibletilescount; j++)
			{
				if(!(file >> x >> y))
				{
					std::cerr << "Error while loading map file " << filename << " : Visibility map data section incomplete." << std::endl;
					return false;
				}
				m_visibilitymap[i][j] = sf::Vector2u(x, y);
			}
		}
	}

	m_name = name;
	m_loaded = true;

	return true;
}

const std::string &Map::getName() const
{
	return m_name;
}

const Tile &Map::getTile(unsigned int x, unsigned int y) const
{
	return m_tiles[y * m_size.x + x];
}

const Tile &Map::getTileByHash(unsigned int xy) const
{
	return m_tiles[xy];
}

const sf::Vector2u &Map::getSize() const
{
	return m_size;
}

const std::vector<sf::Vector2u> &Map::getTilesVisibleFrom(unsigned int x, unsigned int y) const
{
	return m_visibilitymap[y * m_size.x + x];
}

Map::operator bool() const
{
	return m_loaded;
}

void Map::setTile(unsigned int x, unsigned int y, const Tile &tile)
{
	m_tiles[y * m_size.x + x] = tile;
}

bool Map::save(const std::string &name)
{
	const std::string filename = MAP_PREFIX + name + MAP_SUFFIX;
	//Open the map file
	BinaryFile file(filename.c_str(), std::ios::out);
	if(!file)
	{
		std::cerr << "Cannot open map file " << filename << " for writing." << std::endl;
		return false;
	}

	//Append size
	file << (sf::Uint16)m_size.x << (sf::Uint16)m_size.y;
	//Append tiles
	for(unsigned int i = 0; i < m_tiles.size(); i++)
	{
		file << m_tiles[i];
		file << (sf::Uint16)m_visibilitymap[i].size();
		for(const sf::Vector2u &tilecoords : m_visibilitymap[i])
			file << (sf::Uint16)tilecoords.x << (sf::Uint16)tilecoords.y;
	}
	if(!file)
	{
		std::cerr << "Could not write map to file " << filename << ". (Output error.)" << std::endl;
		file.close();
		return false;
	}
	file.close();
	return true;
}
