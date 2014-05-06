#ifndef FOVCOMPUTER_HPP_INCLUDED
#define FOVCOMPUTER_HPP_INCLUDED

#include "Map.hpp"
#include <list>

class FOVComputer
{
	public:
	FOVComputer(const Map &map, std::list<sf::Vector2u> &visibletiles);

	FOVComputer(const FOVComputer &) = delete;
	FOVComputer &operator=(const FOVComputer &) = delete;

	void operator()(unsigned int x, unsigned int y);

	private:
	void castLight(unsigned int row, float startslope, float endslope);

	const Map &m_map;
	unsigned int m_startx;
	unsigned int m_starty;
	std::list<sf::Vector2u> &m_visibletiles;

	int m_xx;
	int m_xy;
	int m_yx;
	int m_yy;
	int m_maxdist;
};

#endif // FOVCOMPUTER_HPP_INCLUDED
