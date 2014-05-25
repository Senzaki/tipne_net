#ifndef DRAWABLESPELLPROJECTILE_HPP_INCLUDED
#define DRAWABLESPELLPROJECTILE_HPP_INCLUDED

#include "SpellProjectileStateListener.hpp"
#include "DrawableEntity.hpp"
#include "SpellProjectile.hpp"
#include "Animator.hpp"

class DrawableSpellProjectile : public DrawableEntity, public SpellProjectileStateListener
{
	public:
	DrawableSpellProjectile(SpellProjectile *projectile);
	virtual ~DrawableSpellProjectile();

	virtual void update(float etime);
	virtual void draw(sf::RenderWindow &window);

	virtual float getDepth() const;
	virtual bool isContainedIn(const sf::FloatRect &rect) const;

	virtual void onPositionChanged(const sf::Vector2f &position);

	private:
	void initAppearance(sf::Uint16 appearance);

	sf::Sprite m_sprite;
	Animator m_animator;
	float m_height;
};

#endif // DRAWABLESPELLPROJECTILE_HPP_INCLUDED
