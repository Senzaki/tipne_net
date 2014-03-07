#include "GameScreen.hpp"
#include "Application.hpp"

GameScreen::GameScreen(float vratio, float xyratio, GameSimulator *simulator):
	m_camera(sf::FloatRect(1000.f, -DEFAULT_SCREEN_HEIGHT / 2, xyratio * DEFAULT_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT)),
	m_seen(sf::FloatRect(1000.f, -DEFAULT_SCREEN_HEIGHT / 2, xyratio * DEFAULT_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT)),
	m_vratio(vratio),
	m_xyratio(xyratio)
{
	setSimulator(simulator);
}

GameScreen::~GameScreen()
{

}

void GameScreen::setSimulator(GameSimulator *simulator)
{
	m_simulator = simulator;
	if(simulator)
	{
		simulator->setStateListener(this);
		//Update the map if there's one
		if(simulator->getMap())
			m_map.setMap(simulator->getMap());
	}
}

bool GameScreen::update(float etime)
{
	bool toreturn = true;
	//Update the simulator (and return false if the simulation is over)
	if(m_simulator)
		toreturn = m_simulator->update(etime);
	m_map.update(etime, m_seen);
	return toreturn;
}

void GameScreen::draw(sf::RenderWindow &window)
{
	//Save the old view and use the game camera
	sf::View oldview = window.getView();
	window.setView(m_camera);
	m_map.draw(window, m_seen);
	//Go back to the old view
	window.setView(oldview);
}

void GameScreen::onNewPlayer(Player &player)
{

}

void GameScreen::onPlayerLeft(Player &player, sf::Uint8 reason)
{

}

void GameScreen::onMapLoaded(const Map &map)
{
	m_map.setMap(map);
}
