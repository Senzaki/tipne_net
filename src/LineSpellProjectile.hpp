#ifndef LINESPELLPROJECTILE_HPP_INCLUDED
#define LINESPELLPROJECTILE_HPP_INCLUDED

#include "SpellProjectile.hpp"

class LineSpellProjectile : public virtual SpellProjectile
{
	public:
	LineSpellProjectile(GameSimulator &simulator, const sf::Vector2f &speed = sf::Vector2f(), const sf::Vector2f &startpos = sf::Vector2f(), float range = 0.f);
	virtual ~LineSpellProjectile();

	LineSpellProjectile(const LineSpellProjectile &) = delete;
	LineSpellProjectile &operator=(const LineSpellProjectile &) = delete;

	void setSpeed(const sf::Vector2f &speed);
	sf::Vector2f getSpeed() const;

	virtual void update(float etime);

	private:
	sf::Vector2f m_start;
	float m_rangesq;
};

#endif // LINESPELLPROJECTILE_HPP_INCLUDED
