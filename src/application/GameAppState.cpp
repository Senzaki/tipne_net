#include "GameAppState.hpp"
#include "Application.hpp"
#include "ResourceManager.hpp"
#include "Translator.hpp"
#include "Config.hpp"
#include "Menu.hpp"

GameAppState::GameAppState(sf::RenderWindow &window, float vratio, float xyratio, std::unique_ptr<GameSimulator> &&simulator):
	m_window(window),
	m_simulator(std::move(simulator)),
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
	rsmgr.unloadSection(ResourceSection::Game);
}

void GameAppState::load()
{
	Translator::getInstance().loadPackage("game");

	//Load all textures & fonts
	ResourceManager &rsmgr = ResourceManager::getInstance();
	rsmgr.loadSection(ResourceSection::Map);
	rsmgr.loadSection(ResourceSection::Game);
	m_cursor.setTexture(rsmgr.getTexture(ResourceSection::Base, Resource::CURSOR_TEX));

	//We can now attach the GameScreen to the GameSimulator
	m_gscr.setSimulator(m_simulator.get());
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

void GameAppState::onFocusLost()
{
	m_gscr.onFocusLost();
}

void GameAppState::onFocusGained()
{
	m_gscr.onFocusGained();
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
				m_gscr.onKeyPressed(evt);
				break;
		}
	}
}

void GameAppState::onKeyReleased(const sf::Event::KeyEvent &evt)
{
	if(!m_guimgr.onKeyReleased(evt))
		m_gscr.onKeyReleased(evt);
}

void GameAppState::onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt)
{
	if(!m_guimgr.onMouseButtonPressed(evt))
		m_gscr.onMouseButtonPressed(evt);
}

void GameAppState::onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt)
{
	if(!m_guimgr.onMouseButtonReleased(evt))
		m_gscr.onMouseButtonReleased(evt);
}

void GameAppState::onMouseMoved(const sf::Event::MouseMoveEvent &evt)
{
	m_guimgr.onMouseMoved(evt);
	m_gscr.onMouseMoved(evt);
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
	auto menu = std::make_shared<Menu>(m_window, m_vratio, m_xyratio);
	Application::getInstance().setNextAppState(menu);
}
