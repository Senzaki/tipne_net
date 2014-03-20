#ifndef CHARACTERSTATELISTENER_HPP_INCLUDED
#define CHARACTERSTATELISTENER_HPP_INCLUDED

#include "Character.hpp"

class CharacterStateListener
{
	public:
	virtual ~CharacterStateListener() {}

	virtual void onStateChanged(Character::State state) {}
	virtual void onPositionChanged(sf::Vector2f position) {}
};

#endif // CHARACTERSTATELISTENER_HPP_INCLUDED
