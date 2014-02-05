#ifndef SIMULATORSTATELISTENER_HPP_INCLUDED
#define SIMULATORSTATELISTENER_HPP_INCLUDED

class SimulatorStateListener
{
	public:
	virtual ~SimulatorStateListener() { }
	virtual void onNewPlayer(Player &player) { }
	virtual void onPlayerLeft(Player &player) { }
};

#endif // SIMULATORSTATELISTENER_HPP_INCLUDED
