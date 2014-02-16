#ifndef SIMULATORSTATELISTENER_HPP_INCLUDED
#define SIMULATORSTATELISTENER_HPP_INCLUDED

#include <SFML/System.hpp>

class SimulatorStateListener
{
	public:
	virtual ~SimulatorStateListener() { }
	virtual void onNewPlayer(Player &player) { }
	virtual void onPlayerLeft(Player &player, sf::Uint8 reason) { }
};

#endif // SIMULATORSTATELISTENER_HPP_INCLUDED
