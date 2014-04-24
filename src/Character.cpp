#include "Character.hpp"
#include "CharacterStateListener.hpp"
#include "GameSimulator.hpp"
#include <cmath>

static const float SQRT_2 = 1.41421356237309504880168872420969807f;
static const float DEFAULT_SPEED = 3.f;

Character::Character(sf::Uint16 id, bool fullysimulated, float interpolationtime, sf::Uint8 owner, sf::Vector2f position, State state):
	m_state(state),
	m_colobj(CollisionEntityType::Character, this, interpolationtime),
	m_id(id),
	m_owner(owner),
	m_fullysimulated(fullysimulated),
	m_listener(nullptr),
	m_simulator(nullptr)
{
	m_colobj.setHalfSize(sf::Vector2f(0.4f, 0.4f));
}

Character::Character(Character &&other):
	m_state(other.m_state),
	m_colobj(std::move(other.m_colobj)),
	m_direction(other.m_direction),
	m_id(other.m_id),
	m_owner(other.m_owner),
	m_fullysimulated(other.m_fullysimulated),
	m_listener(other.m_listener),
	m_simulator(other.m_simulator)
{
	other.m_listener = nullptr;
	other.m_simulator = nullptr;
	//Also change the entity to which the collision objects refers
	m_colobj.setEntity(this);
}

bool Character::loadFromPacket(sf::Packet &packet)
{
	float x, y;
	sf::Uint8 state;
	//Extract the data
	packet >> m_id >> m_owner >> x >> y >> state;
	//Can the state be safely casted to State enum type ?
	if(state >= (sf::Uint8)State::Count)
		return false;
	//Apply the remaining data
	m_colobj.forcePosition(x, y);
	m_state = static_cast<State>(state);
	//Return whether the packet is valid or not
	return packet;
}

void Character::writeToPacket(sf::Packet &packet, bool hideserverinfo) const
{
	packet << m_id << (sf::Uint8)(hideserverinfo ? NEUTRAL_PLAYER : m_owner) << (float)m_colobj.getPosition().x << (float)m_colobj.getPosition().y << (sf::Uint8)m_state;
}

void Character::setId(sf::Uint16 id)
{
	m_id = id;
}

sf::Uint16 Character::getId() const
{
	return m_id;
}

void Character::setCharacterStateListener(CharacterStateListener *listener)
{
	m_listener = listener;
	if(m_listener)
	{
		m_listener->onStateChanged(m_state);
		m_listener->onPositionChanged(m_colobj.getPosition());
	}
}

void Character::setSimulator(GameSimulator *simulator)
{
	m_simulator = simulator;
}

void Character::setCollisionManager(CollisionManager *colmgr)
{
	//Remove the former collision manager (if any)
	if(m_colobj.getCollisionManager())
		m_colobj.getCollisionManager()->detach(&m_colobj);
	//Attach the object to the new one
	if(colmgr)
		colmgr->attach(&m_colobj);
}

void Character::setState(State state)
{
	m_state = state;
	m_listener->onStateChanged(m_state);
}

Character::State Character::getState() const
{
	return m_state;
}

void Character::setOwner(sf::Uint8 owner)
{
	m_owner = owner;
}

sf::Uint8 Character::getOwner() const
{
	return m_owner;
}

void Character::setFullySimulated(bool fullysimulated)
{
	m_fullysimulated = fullysimulated;
}

bool Character::isFullySimulated() const
{
	return m_fullysimulated;
}

void Character::setInterpolationTime(float interpolationtime)
{
	m_colobj.setInterpolationTime(interpolationtime);
}

float Character::getInterpolationTime() const
{
	return m_colobj.getInterpolationTime();
}

void Character::update(float etime)
{
    //Change the position if the character is moving (don't change it if it is not needed, because it would restart the interpolation time, thus preventing the character from really reaching its target)
    if(m_fullysimulated && (m_direction.x != 0.f || m_direction.y != 0.f))
		m_colobj.setPosition(m_colobj.getDesiredPosition() + m_direction * DEFAULT_SPEED * etime);
	if(m_colobj.update(etime))
	{
		//Only tell the listener about the position change if it has really changed
		if(m_listener)
			m_listener->onPositionChanged(m_colobj.getPosition());
	}
}

void Character::setDirection(sf::Vector2f direction)
{
	//Normalize the direction if non-zero
	if(!(direction.x == 0.f && direction.y == 0.f))
		direction = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
	if(m_direction != direction)
	{
		//Only tell the simulator if the direction has changed (to avoid sending extra packets)
		m_direction = direction;
		if(m_simulator)
			m_simulator->onCharacterSpeedChanged(*this, direction);
	}
}

void Character::setPosition(float x, float y)
{
	m_colobj.setPosition(x, y);
}

void Character::forcePosition(float x, float y)
{
	m_colobj.forcePosition(x, y);
}

sf::Vector2f Character::getPosition() const
{
	return m_colobj.getPosition();
}
