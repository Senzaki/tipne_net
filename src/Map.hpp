#ifndef MAP_HPP_INCLUDED
#define MAP_HPP_INCLUDED

#include <vector>
#include "Tile.hpp"
#include <SFML/System.hpp>

enum class MapId : sf::Uint8
{
	Default = 0,
	Count
};

class Map
{
	public:
	Map();
	Map(Map &&other);
	~Map();

	Map(const Map &) = delete;
	Map &operator=(const Map &) = delete;

	Map &operator=(Map &&other);

	bool load(sf::Uint8 mapid);
	sf::Uint8 getID() const;

	Tile &getTile(unsigned int x, unsigned int y);
	const sf::Vector2u &getSize() const;

	operator bool();

	private:
	sf::Uint8 m_id;
	std::vector<Tile> m_tiles;
	sf::Vector2u m_size;
};

#endif // MAP_HPP_INCLUDED
