#ifndef DRAWABLETILESGROUP_HPP_INCLUDED
#define DRAWABLETILESGROUP_HPP_INCLUDED

#include "Map.hpp"
#include "DrawableWall.hpp"
#include "Animator.hpp"
#include <list>

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
	void setTileVisible(unsigned int localx, unsigned int localy);

	private:
	struct TileSet
	{
		const sf::Texture *texture;
		sf::VertexArray vertices;
	};
	std::vector<TileSet> m_tilesets;
	std::vector<bool> m_visibility;
	std::vector<std::tuple<sf::VertexArray *, unsigned int, float>> m_verticesinfo;
	unsigned int m_width;
	std::list<DrawableWall> m_walls;
	std::vector<std::list<DrawableWall *>> m_wallsongrid;
};

#endif // DRAWABLETILESGROUP_HPP_INCLUDED
