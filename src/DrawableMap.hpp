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

	void update(float etime, bool showvision = false, const sf::Vector2f &viewer = sf::Vector2f());
	void draw(sf::RenderWindow &window, const sf::FloatRect &seen, std::list<DrawableEntity *> &mapentities);//Map entities will contain entities that couldn't be drawn because they needed depth test

	bool setMap(const Map &map);

	private:
	const Map *m_map;
	std::vector<DrawableTilesGroup> m_chunks;
	sf::Vector2u m_chunkscount;
	float m_graphetime;
};

#endif // DRAWABLEMAP_HPP_INCLUDED
