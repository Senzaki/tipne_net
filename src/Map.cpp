#include "Map.hpp"
#include <fstream>
#include <SFML/Network.hpp>
#include <iostream>

static constexpr const char *MAP_FILES[] = {"data/maps/default.map"};

Map::Map():
	m_id((sf::Uint8)MapId::Count)
{

}

Map::Map(Map &&other):
	m_id(other.m_id),
	m_tiles(std::move(other.m_tiles)),
	m_size(other.m_size)
{

}

Map::~Map()
{

}

Map &Map::operator=(Map &&other)
{
	m_id = other.m_id;
	m_tiles = std::move(other.m_tiles);
	m_size = other.m_size;
	return *this;
}

bool Map::load(sf::Uint8 mapid)
{
	if(mapid >= (sf::Uint8)MapId::Count)
		return false;
	const char *filename = MAP_FILES[mapid];
	//TODO : Stop using sf::Packet to avoid unuseful copies
	//Open the map file
	std::ifstream file(filename, std::ios::binary);
	if(!file)
	{
		std::cout << "Cannot open map file " << filename << "." << std::endl;
		return false;
	}

	//Use sf::Packet to avoid having to care about endianness etc
	sf::Packet data;
	//Read the size of the map
	{
		constexpr const std::size_t toread = 2 * sizeof(sf::Uint16);
		char buf[toread];
		if(!file.read(buf, toread))
			return false;
		//Add it to the packet & extract numbers
		data.append(buf, toread);
		sf::Uint16 w, h;
		data >> w >> h;
		if(w == 0 || h == 0)
		{
			std::cerr << "Error while loading map file " << filename << " : Map size cannot be 0 !" << std::endl;
			return false;
		}
		m_size.x = w;
		m_size.y = h;
		m_tiles.resize(m_size.x * m_size.y);
		//Reset the packet
		data.clear();
	}
	//Read each tile
	{
		const unsigned int tilescount = m_size.x * m_size.y;
		const std::size_t toread = Tile::getSizeInPacket() * tilescount;
		char *buf = new char[toread];
		if(!file.read(buf, toread))
		{
			delete[] buf;
			std::cerr << "Error while loading map file " << filename << " : Not enough tiles !" << std::endl;
			return false;
		}
		//Add it to the packet & extract tiles
		data.append(buf, toread);
		delete[] buf;
		for(unsigned int i = 0; i < tilescount; i++)
			data >> m_tiles[i];
		//Reset the packet
		data.clear();
	}

	m_id = mapid;

	return true;
}

sf::Uint8 Map::getID() const
{
	return m_id;
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

Map::operator bool() const
{
	return !m_tiles.empty();
}
