#ifndef BASISCHANGE_HPP_INCLUDED
#define BASISCHANGE_HPP_INCLUDED

constexpr const float GRID_WIDTH = 50.f;
constexpr const float GRID_HEIGHT = 25.f;

struct BasisChange
{
	inline static sf::Vector2f pixelToGrid(const sf::Vector2f &vec);
	inline static sf::Vector2f gridToPixel(const sf::Vector2f &vec);
	inline static sf::Vector2f gridToPixel(const sf::Vector2u &vec);

	inline static sf::Vector2f pixelToGrid(float x, float y);
	inline static sf::Vector2f gridToPixel(float x, float y);

	//Partial change
	inline static float pixelToGridX(const sf::Vector2f &vec);
	inline static float pixelToGridY(const sf::Vector2f &vec);
	inline static float gridToPixelX(const sf::Vector2f &vec);
	inline static float gridToPixelY(const sf::Vector2f &vec);
	inline static float gridToPixelX(const sf::Vector2u &vec);
	inline static float gridToPixelY(const sf::Vector2u &vec);

	inline static float pixelToGridX(float x, float y);
	inline static float pixelToGridY(float x, float y);
	inline static float gridToPixelX(float x, float y);
	inline static float gridToPixelY(float x, float y);
};

#include "BasisChange.inl"

#endif // BASISCHANGE_HPP_INCLUDED
