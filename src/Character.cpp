#include "Character.hpp"
#include "CharacterStateListener.hpp"

Character::Character(sf::Uint16 id, bool fullysimulated, float interpolationtime, sf::Uint8 owner, sf::Vector2f position, State state):
	m_state(state),
	m_posmgr(interpolationtime),
	m_id(id),
	m_owner(owner),
	m_fullysimulated(fullysimulated),
	m_listener(nullptr)
{

}

Character::Character(Character &&other):
	m_state(other.m_state),
	m_posmgr(std::move(other.m_posmgr)),
	m_id(other.m_id),
	m_owner(other.m_owner),
	m_fullysimulated(other.m_fullysimulated),
	m_listener(other.m_listener)
{
	other.m_listener = nullptr;
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
	m_posmgr.forcePosition(x, y);
	m_state = static_cast<State>(state);
	//Return wether the packet is valid or not
	return packet;
}

void Character::writeToPacket(sf::Packet &packet, bool hideserverinfo) const
{
	packet << m_id << (sf::Uint8)(hideserverinfo ? NEUTRAL_PLAYER : m_owner) << (float)m_posmgr.getPosition().x << (float)m_posmgr.getPosition().y << (sf::Uint8)m_state;
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
		m_listener->onPositionChanged(m_posmgr.getPosition());
	}
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
	m_posmgr.setInterpolationTime(interpolationtime);
}

float Character::getInterpolationTime() const
{
	return m_posmgr.getInterpolationTime();
}

void Character::update(float etime)
{
	if(m_posmgr.update(etime))
	{
		if(m_listener)
			m_listener->onPositionChanged(m_posmgr.getPosition());
	}
}

sf::Vector2f Character::getPosition() const
{
	return m_posmgr.getPosition();
}
