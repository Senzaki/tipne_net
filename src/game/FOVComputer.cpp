#include "FOVComputer.hpp"
#include "DrawableTilesGroup.hpp"

static const unsigned int MAX_RADIUS = 20;

FOVComputer::FOVComputer(const Map &map, std::list<sf::Vector2u> &visibletiles):
	m_map(map),
	m_visibletiles(visibletiles)
{

}

void FOVComputer::operator()(unsigned int x, unsigned int y)
{
	m_startx = x;
	m_starty = y;

	//Decreasing x
	m_maxdist = std::min(MAX_RADIUS, x);
	m_xx = 0;
	m_xy = 1;
	m_yx = -1;
	m_yy = 0;
	castLight(1, 1.f, 0.f);
	m_xx = 0;
	m_xy = 1;
	m_yx = 1;
	m_yy = 0;
	castLight(1, 1.f, 0.f);

	//Increasing x
	m_maxdist = std::min(MAX_RADIUS, m_map.getSize().x - x);
	m_xx = 0;
	m_xy = -1;
	m_yx = -1;
	m_yy = 0;
	castLight(1, 1.f, 0.f);
	m_xx = 0;
	m_xy = -1;
	m_yx = 1;
	m_yy = 0;
	castLight(1, 1.f, 0.f);

	//Decreasing y
	m_maxdist = std::min(MAX_RADIUS, y);
	m_xx = -1;
	m_xy = 0;
	m_yx = 0;
	m_yy = 1;
	castLight(1, 1.f, 0.f);
	m_xx = 1;
	m_xy = 0;
	m_yx = 0;
	m_yy = 1;
	castLight(1, 1.f, 0.f);

	//Increasing y
	m_maxdist = std::min(MAX_RADIUS, m_map.getSize().y - y);
	m_xx = -1;
	m_xy = 0;
	m_yx = 0;
	m_yy = -1;
	castLight(1, 1.f, 0.f);
	m_xx = 1;
	m_xy = 0;
	m_yx = 0;
	m_yy = -1;
	castLight(1, 1.f, 0.f);
}

void FOVComputer::castLight(unsigned int row, float startslope, float endslope)
{
	if(startslope <= endslope)
		return;
	float nextstartslope = 0.f;
	bool blocked = false;
	for(int distance = row; distance <= m_maxdist && !blocked; distance++)
	{
		int dy = -distance;
		for(int dx = -distance; dx <= 0; dx++)
		{
			//Compute map coords
			int curx = m_startx + dx * m_xx + dy * m_xy;
			int cury = m_starty + dx * m_yx + dy * m_yy;
			//Compute top/bottom slopes
			float lslope = (dx - 0.5f) / (dy + 0.5f);
			float rslope = (dx + 0.5f) / (dy - 0.5f);
			//Skip this cell if it should not be traversed by this func (or if out of the map)
			if(curx < 0 || cury < 0 || curx >= (int)m_map.getSize().x || cury >= (int)m_map.getSize().y || startslope < rslope)
				continue;
			if(endslope > lslope)
				break;
			//Light the cell
			m_visibletiles.emplace_back(curx, cury);
			//Previous cell was blocking ?
			if(blocked)
			{
				//This one as well ?
				if(Map::isAppearanceLightBlocking(m_map.getTile(curx, cury).appearance))
				{

					nextstartslope = rslope;
					continue;
				}
				else
				{
					blocked = false;
					startslope = nextstartslope;
				}
			}
			//Cell blocking with non-blocking cells before ?
			else if(Map::isAppearanceLightBlocking(m_map.getTile(curx, cury).appearance))
			{
				blocked = true;
				castLight(distance + 1, startslope, lslope);
				nextstartslope = rslope;
			}
		}
	}
}
