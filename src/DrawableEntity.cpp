#include "DrawableEntity.hpp"
#include "Math.hpp"
#include "BasisChange.hpp"
#include <cfloat>

static constexpr float DIRECTIONS[(int)IsometricDirection::Count][2] =
{
	{-INVSQRT_2, INVSQRT_2},
	{0, 1.f},
	{INVSQRT_2, INVSQRT_2},
	{1.f, 0},
	{INVSQRT_2, -INVSQRT_2}
};

void DrawableEntity::getDirectionInfo(sf::Vector2f direction, sf::Vector2f &scale, IsometricDirection &closestdir)
{
	//Flip ?
	scale.y = 1.f;
	direction = BasisChange::gridToPixel(direction);
	if(direction.x < 0.f)
	{
		direction.x = -direction.x;
		scale.x = -1.f;
	}
	else
		scale.x = 1.f;
	direction = BasisChange::pixelToGrid(direction);

	//Compute closest direction
	float mindistsq = FLT_MAX;
	float dx;
	float dy;
	float distsq;
    for(unsigned int i = 0; i < (int)IsometricDirection::Count; i++)
    {
		dx = direction.x - DIRECTIONS[i][0];
		dy = direction.y - DIRECTIONS[i][1];
		distsq = dx * dx + dy * dy;
		if(distsq < mindistsq)
		{
			mindistsq = distsq;
			closestdir = (IsometricDirection)i;
		}
    }
}
