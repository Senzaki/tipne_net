#ifndef LINESPELLPROJECTILE_HPP_INCLUDED
#define LINESPELLPROJECTILE_HPP_INCLUDED

#include "GameEntity.hpp"

class LineSpellProjectile : public virtual GameEntity
{
	public:
	LineSpellProjectile(GameSimulator &simulator, sf::Vector2f speed = sf::Vector2f());
	virtual ~LineSpellProjectile();

	LineSpellProjectile(const LineSpellProjectile &) = delete;
	LineSpellProjectile &operator=(const LineSpellProjectile &) = delete;

	void setSpeed(float x, float y);
	const sf::Vector2f &getSpeed() const;

	virtual void update(float etime);

	private:
	sf::Vector2f m_speed;
};

#endif // LINESPELLPROJECTILE_HPP_INCLUDED
