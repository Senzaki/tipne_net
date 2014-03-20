#include "GameScreen.hpp"
#include "Application.hpp"

GameScreen::GameScreen(float vratio, float xyratio, GameSimulator *simulator):
	m_camera(sf::FloatRect(-100.f, -DEFAULT_SCREEN_HEIGHT / 2, xyratio * DEFAULT_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT)),
	m_seen(sf::FloatRect(-100.f, -DEFAULT_SCREEN_HEIGHT / 2, xyratio * DEFAULT_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT)),
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
		simulator->setStateListener(this);
}

bool GameScreen::update(float etime)
{
	bool toreturn = true;
	//Update the simulator (and return false if the simulation is over)
	if(m_simulator)
		toreturn = m_simulator->update(etime);
	//Update the graphical entities
	m_map.update(etime, m_seen);
	for(std::pair<const sf::Uint16, DrawableCharacter> &character : m_characters)
		character.second.update(etime);
	return toreturn;
}

void GameScreen::draw(sf::RenderWindow &window)
{
	//Save the old view and use the game camera
	sf::View oldview = window.getView();
	window.setView(m_camera);
	//Draw the graphical entities
	m_map.draw(window, m_seen);
	for(std::pair<const sf::Uint16, DrawableCharacter> &character : m_characters)
		character.second.draw(window);
	//Go back to the old view
	window.setView(oldview);
}

void GameScreen::onNewPlayer(Player &player)
{

}

void GameScreen::onPlayerLeft(Player &player, sf::Uint8 reason)
{

}

void GameScreen::onNewCharacter(Character &character)
{
	m_characters.emplace(character.getId(), character);
}

void GameScreen::onCharacterRemoved(Character &character)
{
	m_characters.erase(character.getId());
}

void GameScreen::onMapLoaded(const Map &map)
{
	m_map.setMap(map);
}
