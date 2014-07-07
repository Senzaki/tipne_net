#ifndef DRAWABLEWALL_HPP_INCLUDED
#define DRAWABLEWALL_HPP_INCLUDED

#include "DrawableEntity.hpp"

class DrawableWall : public DrawableEntity
{
	public:
	DrawableWall(const sf::Vector2u &position, sf::Uint16 appearance);
	virtual ~DrawableWall();

	void setLight(const sf::Color &color);

	virtual float getDepth() const;
	virtual bool isContainedIn(const sf::FloatRect &rect) const;
	virtual void draw(sf::RenderWindow &window);

	private:
	sf::Sprite m_sprite;
	float m_depth;
};

#endif // DRAWABLEWALL_HPP_INCLUDED
