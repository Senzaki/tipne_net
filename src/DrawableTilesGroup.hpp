#ifndef DRAWABLETILESGROUP_HPP_INCLUDED
#define DRAWABLETILESGROUP_HPP_INCLUDED

#include "Map.hpp"
#include "DrawableWall.hpp"
#include "Animator.hpp"
#include <list>

static constexpr sf::Uint16 FIRST_WALL_APPEARANCE = 0x100;

struct GraphTileInfo
{
	unsigned int texture;
	unsigned int framescount;
	Animator::Frame frames[16];
};

class DrawableTilesGroup
{
	public:
	DrawableTilesGroup();

	bool loadTiles(const Map &map, const sf::Rect<unsigned int> &rect);

	void update(float etime);
	void draw(sf::RenderWindow &window);
	std::list<DrawableWall> &getWalls();

	private:
	struct TileSet
	{
		const sf::Texture *texture;
		sf::VertexArray vertices;
	};
	std::vector<TileSet> m_tilesets;
	std::list<DrawableWall> m_walls;
};

#endif // DRAWABLETILESGROUP_HPP_INCLUDED
