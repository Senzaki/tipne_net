#ifndef DRAWABLETILESGROUP_HPP_INCLUDED
#define DRAWABLETILESGROUP_HPP_INCLUDED

#include "Map.hpp"
#include <SFML/Graphics.hpp>

class DrawableTilesGroup
{
	public:
	DrawableTilesGroup();

	bool loadTiles(const Map &map, const sf::Rect<unsigned int> &rect);

	void update(float etime);
	void draw(sf::RenderWindow &window);

	private:
	struct TileSet
	{
		const sf::Texture *texture;
		sf::VertexArray vertices;
	};
	std::vector<TileSet> m_statictiles;
};

#endif // DRAWABLETILESGROUP_HPP_INCLUDED
