#ifndef CHARACTER_HPP_INCLUDED
#define CHARACTER_HPP_INCLUDED

#include "Player.hpp"
#include "GameEntity.hpp"

class GameSimulator;
class CharacterStateListener;
class CollisionManager;//Reduce compile time

constexpr const float DEFAULT_CHARACTER_RADIUS = 0.45f;

class Character : public GameEntity
{
	public:
	enum class State
	{
		Ghost = 0,
		Count
	};

	Character(GameSimulator &simulator, sf::Uint16 id, State state = State::Ghost);
	virtual ~Character();

	Character(const Character &) = delete;
	Character &operator=(const Character &) = delete;

	void setCharacterStateListener(CharacterStateListener *listener);

	void setState(State state);
	State getState() const;

	virtual void update(float etime);

	void setDirection(sf::Vector2f direction);
	sf::Vector2f getDirection() const;

	private:
	State m_state;
	sf::Vector2f m_direction;

	CharacterStateListener *m_listener;
};

#endif // CHARACTER_HPP_INCLUDED
