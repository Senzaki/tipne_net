#ifndef DRAWABLECHARACTER_HPP_INCLUDED
#define DRAWABLECHARACTER_HPP_INCLUDED

#include "CharacterStateListener.hpp"
#include <SFML/Graphics.hpp>

class DrawableCharacter : public CharacterStateListener
{
	public:
	DrawableCharacter();
	DrawableCharacter(Character &character);
	virtual ~DrawableCharacter();

	void update(float etime);
	void draw(sf::RenderWindow &window);

	sf::Vector2f getPosition() const;

	virtual void onPositionChanged(sf::Vector2f position);
	virtual void onStatusChanged(Character::State state);

	private:
	sf::Sprite m_sprite;
};

#endif // DRAWABLECHARACTER_HPP_INCLUDED
