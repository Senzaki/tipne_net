#ifndef POSITIONMANAGER_HPP_INCLUDED
#define POSITIONMANAGER_HPP_INCLUDED

#include <SFML/System.hpp>

constexpr const float CLIENT_INTERPOLATION_TIME = 0.1f;

class PositionManager
{
	public:
	PositionManager(float interpolationtime = 0.f);//0 means no interpolation

	void setPosition(float x, float y);//Sets "wanted" position, but it may not be the position returned by getPosition (e.g. interpolation)
	void setPosition(const sf::Vector2f &position);
	void forcePosition(float x, float y);//This one will force the position (no interpolation possible)
	void forcePosition(const sf::Vector2f &position);
	sf::Vector2f getPosition() const;

	void setInterpolationTime(float interpolationtime);
	float getInterpolationTime() const;

	bool isStatic() const;

	bool update(float etime);//Returns true if the position has changed

	private:
	sf::Vector2f m_start;
	sf::Vector2f m_direction;
	sf::Vector2f m_position;
	float m_maxtime;
	float m_time;
};

#endif // POSITIONMANAGER_HPP_INCLUDED
