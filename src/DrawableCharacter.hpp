#ifndef DRAWABLECHARACTER_HPP_INCLUDED
#define DRAWABLECHARACTER_HPP_INCLUDED

#include "CharacterStateListener.hpp"
#include "DrawableEntity.hpp"

class DrawableCharacter : public CharacterStateListener, public DrawableEntity
{
	public:
	DrawableCharacter();
	DrawableCharacter(Character &character);
	virtual ~DrawableCharacter();

	void update(float etime);
	virtual void draw(sf::RenderWindow &window);

	sf::Vector2f getPosition() const;
	virtual float getDepth() const;
	virtual bool isContainedIn(const sf::FloatRect &rect) const;

	virtual void onPositionChanged(sf::Vector2f position);
	virtual void onStatusChanged(Character::State state);

	private:
	sf::Sprite m_sprite;
	float m_depth;
};

#endif // DRAWABLECHARACTER_HPP_INCLUDED
