#ifndef SIMULATORSTATELISTENER_HPP_INCLUDED
#define SIMULATORSTATELISTENER_HPP_INCLUDED

#include "Map.hpp"

class SimulatorStateListener
{
	public:
	virtual ~SimulatorStateListener() { }
	virtual void onNewPlayer(Player &player) { }
	virtual void onPlayerLeft(Player &player, sf::Uint8 reason) { }
	virtual void onMapLoaded(const Map &map) { }
};

#endif // SIMULATORSTATELISTENER_HPP_INCLUDED
