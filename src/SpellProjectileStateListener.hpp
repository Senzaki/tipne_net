#ifndef SPELLPROJECTILESTATELISTENER_HPP_INCLUDED
#define SPELLPROJECTILESTATELISTENER_HPP_INCLUDED

#include <SFML/System.hpp>

class SpellProjectileStateListener
{
	public:
	virtual ~SpellProjectileStateListener() { }

	virtual void onPositionChanged(const sf::Vector2f &position) { }
};

#endif // SPELLPROJECTILESTATELISTENER_HPP_INCLUDED
