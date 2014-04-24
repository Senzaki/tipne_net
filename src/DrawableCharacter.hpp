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
	void initializeBaseCircle();

	sf::Sprite m_sprite;
	sf::Transform m_transform;
	sf::VertexArray m_basecircle;
	sf::FloatRect m_localbounds;
	sf::FloatRect m_bounds;
	float m_depth;
};

#endif // DRAWABLECHARACTER_HPP_INCLUDED
