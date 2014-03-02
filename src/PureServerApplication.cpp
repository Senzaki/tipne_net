#include "PureServerApplication.hpp"
#include "Config.hpp"
#include "NetworkCodes.hpp"
#include <iostream>
#include <csignal>

static const float CYCLE_TIME = 1.f / 80.f;

static void quitPureServer(int signal)
{
	PureServerApplication::getInstance().quit();
}

PureServerApplication &PureServerApplication::getInstance()
{
	static PureServerApplication app;
	return app;
}

PureServerApplication::PureServerApplication():
	m_running(false),
	m_simulator(nullptr)
{

}

int PureServerApplication::execute(int argc, char **argv)
{
	//Handle SIGINT (Ctrl + C) and SIGTERM (kill), so that the application can quit cleanly
	signal(SIGINT, &quitPureServer);
	signal(SIGTERM, &quitPureServer);
	//Setup the simulator
	m_simulator = new ServerSimulator(true);
	m_simulator->setStateListener(this);

	//TEMP
	if(!m_simulator->loadMap((sf::Uint8)MapId::Default))
		return 1;
	//////

	if(!m_simulator->startNetThread(Config::getInstance().server_port, Config::getInstance().max_players))
	{
		std::cerr << "Cannot start server." << std::endl;
		return 1;
	}

	m_running = true;
	sf::Clock clock;
	float etime;
	while(m_running)
	{
		//Sleep if required
		etime = clock.getElapsedTime().asSeconds();
		if(etime < CYCLE_TIME)
		{
			sf::sleep(sf::seconds(CYCLE_TIME - etime));
			etime = clock.getElapsedTime().asSeconds();
		}
		clock.restart();

		//Simulate the world
		m_simulator->update(etime);
	}

	delete m_simulator;
	m_simulator = nullptr;

	return 0;
}

void PureServerApplication::quit()
{
	m_running = false;
}


void PureServerApplication::onNewPlayer(Player &player)
{
	std::cout << player.name << " joined the game." << std::endl;
}

void PureServerApplication::onPlayerLeft(Player &player, sf::Uint8 reason)
{
	switch(reason)
	{
		case (sf::Uint8)DisconnectionReason::Left:
			std::cout << player.name << " left the game." << std::endl;
			break;

		case (sf::Uint8)DisconnectionReason::Error:
			std::cout << player.name << " was disconnected from the game." << std::endl;
			break;

		case (sf::Uint8)DisconnectionReason::Kicked:
			std::cout << player.name << " was kicked from the game." << std::endl;
			break;
	}
}

void PureServerApplication::onMapLoaded(const Map &map)
{
	std::cout << "New map loaded." << std::endl;
}
