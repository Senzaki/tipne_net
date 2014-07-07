#ifndef CHARACTERSTATELISTENER_HPP_INCLUDED
#define CHARACTERSTATELISTENER_HPP_INCLUDED

#include "Character.hpp"

class CharacterStateListener
{
	public:
	virtual ~CharacterStateListener() {}

	virtual void onStateChanged(Character::State state) {}
	virtual void onPositionChanged(const sf::Vector2f &position) {}
	virtual void onDirectionChanged(const sf::Vector2f &direction) {}
};

#endif // CHARACTERSTATELISTENER_HPP_INCLUDED
