#include "Map.hpp"
#include <fstream>
#include <SFML/Network.hpp>
#include <iostream>

static const std::string MAP_PREFIX = "data/maps/";
static const std::string MAP_SUFFIX = ".map";

Map::Map()
{

}

Map::Map(Map &&other):
	m_name(std::move(other.m_name)),
	m_tiles(std::move(other.m_tiles)),
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
	const std::string filename = MAP_PREFIX + name + MAP_SUFFIX;
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

	m_name = name;

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

Map::operator bool() const
{
	return !m_tiles.empty();
}

void Map::setTile(unsigned int x, unsigned int y, const Tile &tile)
{
	m_tiles[y * m_size.x + x] = tile;
}

bool Map::save(const std::string &name)
{
	const std::string filename = MAP_PREFIX + name + MAP_SUFFIX;
	//Open the map file
	std::ofstream file(filename, std::ios::binary);
	if(!file)
	{
		std::cerr << "Cannot open map file " << filename << " for writing." << std::endl;
		return false;
	}

	sf::Packet data;
	//Append size
	data << (sf::Uint16)m_size.x << (sf::Uint16)m_size.y;
	//Append tiles
	for(unsigned int i = 0; i < m_tiles.size(); i++)
		data << m_tiles[i];
	//Write to file
	file.write((char *)data.getData(), data.getDataSize());
	if(!file)
	{
		std::cerr << "Could not write map to file " << filename << ". (Output error.)" << std::endl;
		file.close();
		return false;
	}
	file.close();
	return true;
}
