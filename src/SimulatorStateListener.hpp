#ifndef SIMULATORSTATELISTENER_HPP_INCLUDED
#define SIMULATORSTATELISTENER_HPP_INCLUDED

#include "Map.hpp"
#include "GameEntity.hpp"
#include "Player.hpp"
#include <list>

class SimulatorStateListener
{
	public:
	virtual ~SimulatorStateListener() { }
	virtual void onNewPlayer(Player &player) { }
	virtual void onPlayerLeft(Player &player, sf::Uint8 reason) { }
	virtual void onNewEntity(GameEntity *entity) { }
	virtual void onEntityRemoved(GameEntity *entity) { }
	virtual void onMapLoaded(const Map &map) { }
	virtual void onVisibleEntitiesChanged(std::list<sf::Uint16> &&characters) { }
};

#endif // SIMULATORSTATELISTENER_HPP_INCLUDED
