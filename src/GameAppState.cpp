#include "GameAppState.hpp"
#include "Application.hpp"
#include "ResourceManager.hpp"
#include "Translator.hpp"
#include "Config.hpp"
#include "Menu.hpp"

GameAppState::GameAppState(sf::RenderWindow &window, float vratio, float xyratio, GameSimulator *simulator):
	m_window(window),
	m_simulator(simulator),
	m_gscr(vratio, xyratio),
	m_guimgr(window),
	m_camera(sf::FloatRect(0.f, 0.f, xyratio * DEFAULT_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT)),
	m_vratio(vratio),
	m_xyratio(xyratio)
{

}

GameAppState::~GameAppState()
{
	ResourceManager &rsmgr = ResourceManager::getInstance();
	rsmgr.unloadSection(ResourceSection::Map);
	rsmgr.unloadSection(ResourceSection::Characters);
	delete m_simulator;
}

void GameAppState::load()
{
	Translator::getInstance().loadPackage("game");

	//Load all textures & fonts
	ResourceManager &rsmgr = ResourceManager::getInstance();
	rsmgr.loadSection(ResourceSection::Map);
	rsmgr.loadSection(ResourceSection::Characters);
	m_cursor.setTexture(rsmgr.getTexture(ResourceSection::Base, Resource::CURSOR_TEX));

	//We can now attach the GameScreen to the GameSimulator
	m_gscr.setSimulator(m_simulator);
}

void GameAppState::update(float etime)
{
	m_guimgr.update(etime);
	if(!m_gscr.update(etime))
		quit();
}

void GameAppState::draw()
{
	m_window.clear();
	//Draw the game screen
	m_gscr.draw(m_window);
	//Draw GUI
	m_guimgr.draw();
	//Apply the scaling view
	m_window.setView(m_camera);
	//Draw every scaled thing
	m_window.draw(m_cursor);
	//Back to default view
	m_window.setView(m_window.getDefaultView());
}

void GameAppState::onWindowClosed()
{
	Application::getInstance().setNextAppState(nullptr);
}

void GameAppState::onKeyPressed(const sf::Event::KeyEvent &evt)
{
	if(!m_guimgr.onKeyPressed(evt))
	{
		switch(evt.code)
		{
			case sf::Keyboard::Escape:
				Application::getInstance().setNextAppState(nullptr);
				break;

			default:
				break;
		}
	}
}

void GameAppState::onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt)
{
	m_guimgr.onMouseButtonPressed(evt);
	sf::Vector2f camcoords(evt.x / m_vratio, evt.y / m_vratio);
}

void GameAppState::onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt)
{
	m_guimgr.onMouseButtonReleased(evt);
	sf::Vector2f camcoords(evt.x / m_vratio, evt.y / m_vratio);
}

void GameAppState::onMouseMoved(const sf::Event::MouseMoveEvent &evt)
{
	m_guimgr.onMouseMoved(evt);
	sf::Vector2f camcoords(evt.x / m_vratio, evt.y / m_vratio);
	//Cursor is displayed in camera coords, but mouse coordinates are given in window coordinates
	m_cursor.setPosition(camcoords);
}

void GameAppState::onTextEntered(const sf::Event::TextEvent &evt)
{
	m_guimgr.onTextEntered(evt);
}

void GameAppState::quit()
{
	Menu *menu = new Menu(m_window, m_vratio, m_xyratio);
	Application::getInstance().setNextAppState(menu);
}
