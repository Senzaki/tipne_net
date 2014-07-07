#ifndef DRAWABLECHARACTER_HPP_INCLUDED
#define DRAWABLECHARACTER_HPP_INCLUDED

#include "CharacterStateListener.hpp"
#include "DrawableEntity.hpp"
#include "Animator.hpp"

class DrawableCharacter : public CharacterStateListener, public DrawableEntity
{
	public:
	DrawableCharacter(Character *character);
	virtual ~DrawableCharacter();

	virtual void update(float etime);
	virtual void draw(sf::RenderWindow &window);

	virtual float getDepth() const;
	virtual bool isContainedIn(const sf::FloatRect &rect) const;

	virtual void onStateChanged(Character::State state);
	virtual void onPositionChanged(const sf::Vector2f &position);
	virtual void onDirectionChanged(const sf::Vector2f &direction);

	private:
	void initializeBaseCircle();
	void resetAnimation();

	sf::Sprite m_sprite;
	Animator m_animator;
	sf::Transform m_transform;
	sf::VertexArray m_basecircle;
	sf::FloatRect m_localbounds;
	sf::FloatRect m_bounds;
	float m_depth;
	IsometricDirection m_direction;
	bool m_moving;
	Character::State m_state;
};

#endif // DRAWABLECHARACTER_HPP_INCLUDED
