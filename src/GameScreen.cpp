#include "GameScreen.hpp"
#include "Application.hpp"
#include "KeyMap.hpp"
#include "BasisChange.hpp"

GameScreen::GameScreen(float vratio, float xyratio, GameSimulator *simulator):
	m_camera(sf::FloatRect(-100.f, -DEFAULT_SCREEN_HEIGHT / 2, xyratio * DEFAULT_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT)),
	m_seen(sf::FloatRect(-100.f, -DEFAULT_SCREEN_HEIGHT / 2, xyratio * DEFAULT_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT)),
	m_vratio(vratio),
	m_xyratio(xyratio),
	m_otherdirpressed(false, false)
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

void GameScreen::onKeyPressed(const sf::Event::KeyEvent &evt)
{
	//Interpret key
	KeyAction action = KeyMap::getInstance().getActionForKey(evt);
	switch(action)
	{
		case KeyAction::None:
		case KeyAction::Count:
			return;

		case KeyAction::Left:
			//If the current direction is right, remember that this key was pressed
			if(m_direction.x == GRID_WIDTH)
				m_otherdirpressed.x = true;
			//Set the direction to left
			m_direction.x = -GRID_WIDTH;
			updateDirection();
			break;

		case KeyAction::Right:
			//See case KeyAction::Left
			if(m_direction.x == -GRID_WIDTH)
				m_otherdirpressed.x = true;
			m_direction.x = GRID_WIDTH;
			updateDirection();
			break;

		case KeyAction::Up:
			//See case KeyAction::Left
			if(m_direction.y == GRID_HEIGHT)
				m_otherdirpressed.y = true;
			m_direction.y = -GRID_HEIGHT;
			updateDirection();
			break;

		case KeyAction::Down:
			//See case KeyAction::Left
			if(m_direction.y == -GRID_HEIGHT)
				m_otherdirpressed.y = true;
			m_direction.y = GRID_HEIGHT;
			updateDirection();
			break;
	}
}

void GameScreen::onKeyReleased(const sf::Event::KeyEvent &evt)
{
	//Interpret key
	KeyAction action = KeyMap::getInstance().getActionForKey(evt);
	switch(action)
	{
		case KeyAction::None:
		case KeyAction::Count:
			return;

		case KeyAction::Left:
			//If moving to the left
			if(m_direction.x == -GRID_WIDTH)
			{
				//If the right was pressed, go back to right
				if(m_otherdirpressed.x)
					m_direction.x = GRID_WIDTH;
				else
					m_direction.x = 0.f;
				updateDirection();
			}
			//No matter what, one of the two keys has been released, so the other direction is not pressed !
			m_otherdirpressed.x = false;
			updateDirection();
			break;

		case KeyAction::Right:
			//See case KeyAction::Left
			if(m_direction.x == GRID_WIDTH)
			{
				if(m_otherdirpressed.x)
					m_direction.x = -GRID_WIDTH;
				else
					m_direction.x = 0.f;
			}
			m_otherdirpressed.x = false;
			updateDirection();
			break;

		case KeyAction::Up:
			//See case KeyAction::Left
			if(m_direction.y == -GRID_HEIGHT)
			{
				if(m_otherdirpressed.y)
					m_direction.y = GRID_HEIGHT;
				else
					m_direction.y = 0.f;
			}
			m_otherdirpressed.y = false;
			updateDirection();
			break;

		case KeyAction::Down:
			//See case KeyAction::Left
			if(m_direction.y == GRID_HEIGHT)
			{
				if(m_otherdirpressed.y)
					m_direction.y = -GRID_HEIGHT;
				else
					m_direction.y = 0.f;
			}
			m_otherdirpressed.y = false;
			updateDirection();
			break;
	}
}

void GameScreen::onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt)
{

}

void GameScreen::onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt)
{

}

void GameScreen::onMouseMoved(const sf::Event::MouseMoveEvent &evt)
{

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

void GameScreen::updateDirection()
{
	if(m_simulator)
		m_simulator->selfSetDirection(BasisChange::pixelToGrid(m_direction));
}
