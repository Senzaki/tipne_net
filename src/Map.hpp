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

	bool load(const std::string &name);
	const std::string &getName() const;

	const Tile &getTile(unsigned int x, unsigned int y) const;
	const Tile &getTileByHash(unsigned int xy) const; //Retrieve it passing y * size.x + x
	const sf::Vector2u &getSize() const;
	const std::vector<sf::Vector2u> &getTilesVisibleFrom(unsigned int x, unsigned int y) const;

	operator bool() const;

	//For the editor
	void setTile(unsigned int x, unsigned int y, const Tile &tile);
	bool save(const std::string &name);

	private:
	bool m_loaded;
	std::string m_name;
	std::vector<Tile> m_tiles;
	std::vector<std::vector<sf::Vector2u>> m_visibilitymap;
	sf::Vector2u m_size;
};

#endif // MAP_HPP_INCLUDED
