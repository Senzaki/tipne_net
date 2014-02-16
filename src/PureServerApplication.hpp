#ifndef PURESERVERAPPLICATION_HPP_INCLUDED
#define PURESERVERAPPLICATION_HPP_INCLUDED

#include "ServerSimulator.hpp"

class PureServerApplication : public SimulatorStateListener
{
	public:
	static PureServerApplication &getInstance();
	virtual ~PureServerApplication() { }

	PureServerApplication(const PureServerApplication &) = delete;
	PureServerApplication &operator=(const PureServerApplication &) = delete;

	int execute(int argc, char **argv);

	void quit();

	virtual void onNewPlayer(Player &player);
	virtual void onPlayerLeft(Player &player, sf::Uint8 reason);

	private:
	PureServerApplication();

	bool m_running;

	ServerSimulator *m_simulator;
};

#endif // PURESERVERAPPLICATION_HPP_INCLUDED
