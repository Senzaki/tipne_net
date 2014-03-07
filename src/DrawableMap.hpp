#ifndef DRAWABLEMAP_HPP_INCLUDED
#define DRAWABLEMAP_HPP_INCLUDED

#include <SFML/Graphics.hpp>
#include "Map.hpp"
#include "DrawableTilesGroup.hpp"

class DrawableMap
{
	public:
	DrawableMap();

	DrawableMap(const DrawableMap &) = delete;
	DrawableMap &operator=(const DrawableMap &) = delete;

	void update(float etime, sf::FloatRect seen);
	void draw(sf::RenderWindow &window, sf::FloatRect seen);

	bool setMap(const Map &map);

	private:
	std::vector<DrawableTilesGroup> m_chunks;
	sf::Vector2u m_chunkscount;
};

#endif // DRAWABLEMAP_HPP_INCLUDED
