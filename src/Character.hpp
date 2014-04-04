#ifndef CHARACTER_HPP_INCLUDED
#define CHARACTER_HPP_INCLUDED

#include "PositionManager.hpp"
#include "Player.hpp"
class GameSimulator;
class CharacterStateListener;

constexpr const sf::Uint16 NO_CHARACTER_ID = 0;

class Character
{
	public:
	enum class State
	{
		Ghost = 0,
		Human,
		Count
	};

	Character(sf::Uint16 id = NO_CHARACTER_ID, bool fullysimulated = true, float interpolationtime = 0.f, sf::Uint8 owner = NEUTRAL_PLAYER, sf::Vector2f position = sf::Vector2f(), State state = State::Ghost);
	Character(Character &&other);

	bool loadFromPacket(sf::Packet &packet);
	void writeToPacket(sf::Packet &packet, bool hideserverinfo) const;

	Character(const Character &) = delete;
	Character &operator=(const Character &) = delete;

	void setId(sf::Uint16 id);
	sf::Uint16 getId() const;

	void setCharacterStateListener(CharacterStateListener *listener);
	void setSimulator(GameSimulator *simulator);

	void setState(State state);
	State getState() const;
	void setOwner(sf::Uint8 owner);
	sf::Uint8 getOwner() const;

	//Should it be fully simulated, or should we wait for a "server" (or any full-auth system) to tell it what to do ? (note : it is possible to combine both -> extrapolation)
	void setFullySimulated(bool fullysimulated);
	bool isFullySimulated() const;
	void setInterpolationTime(float interpolationtime);
	float getInterpolationTime() const;

	void update(float etime);

	void setDirection(sf::Vector2f direction);

	sf::Vector2f getPosition() const;

	private:
	State m_state;
	PositionManager m_posmgr;
	sf::Vector2f m_direction;

	sf::Uint16 m_id;
	sf::Uint8 m_owner;
	bool m_fullysimulated;
	CharacterStateListener *m_listener;
	GameSimulator *m_simulator;
};

sf::Packet &operator<<(sf::Packet &packet, const Character &character);

#endif // CHARACTER_HPP_INCLUDED
