#ifndef DRAWABLEENTITY_HPP_INCLUDED
#define DRAWABLEENTITY_HPP_INCLUDED

#include <SFML/Graphics.hpp>

enum class IsometricDirection
{
	Down = 0,
	DownRight,
	Right,
	UpRight,
	Up,
	Count
};

class DrawableEntity
{
	public:
	virtual ~DrawableEntity() { }

	virtual float getDepth() const = 0;//Return the lowest y position (in pixel coordinates)
	virtual bool isContainedIn(const sf::FloatRect &rect) const = 0;
	virtual void draw(sf::RenderWindow &window) = 0;

	static inline bool isDepthLower(const DrawableEntity *a, const DrawableEntity *b);
	static void getDirectionInfo(sf::Vector2f direction, sf::Vector2f &scale, IsometricDirection &closestdir);
};

#include "DrawableEntity.inl"

#endif // DRAWABLEENTITY_HPP_INCLUDED
