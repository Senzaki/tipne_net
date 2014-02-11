#include "GameScreen.hpp"
#include "Application.hpp"
#include "ResourceManager.hpp"
#include "Translator.hpp"
#include "Config.hpp"

GameScreen::GameScreen(sf::RenderWindow &window, float vratio, float xyratio, GameSimulator *simulator):
	m_window(window),
	m_simulator(simulator),
	m_guimgr(window),
	m_camera(sf::FloatRect(0.f, 0.f, xyratio * DEFAULT_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT)),
	m_vratio(vratio),
	m_xyratio(xyratio)
{

}

GameScreen::~GameScreen()
{
	delete m_simulator;
}

void GameScreen::load()
{
	Translator::getInstance().loadPackage("game");

	//Load all textures & fonts
	ResourceManager &rsmgr = ResourceManager::getInstance();
	m_cursor.setTexture(rsmgr.getTexture(ResourceSection::Base, Resource::CURSOR_TEX));
}

void GameScreen::update(float etime)
{
	m_guimgr.update(etime);
	m_simulator->update(etime);
}

void GameScreen::draw()
{
	m_window.clear();
	//Draw GUI
	m_guimgr.draw();
	//Apply the scaling view
	m_window.setView(m_camera);
	//Draw every scaled thing
	m_window.draw(m_cursor);
	//Back to default view
	m_window.setView(m_window.getDefaultView());
}

void GameScreen::onWindowClosed()
{
	Application::getInstance().setNextAppState(nullptr);
}

void GameScreen::onKeyPressed(const sf::Event::KeyEvent &evt)
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

void GameScreen::onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt)
{
	m_guimgr.onMouseButtonPressed(evt);
	sf::Vector2f camcoords(evt.x / m_vratio, evt.y / m_vratio);
}

void GameScreen::onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt)
{
	m_guimgr.onMouseButtonReleased(evt);
	sf::Vector2f camcoords(evt.x / m_vratio, evt.y / m_vratio);
}

void GameScreen::onMouseMoved(const sf::Event::MouseMoveEvent &evt)
{
	m_guimgr.onMouseMoved(evt);
	sf::Vector2f camcoords(evt.x / m_vratio, evt.y / m_vratio);
	//Cursor is displayed in camera coords, but mouse coordinates are given in window coordinates
	m_cursor.setPosition(camcoords);
}

void GameScreen::onTextEntered(const sf::Event::TextEvent &evt)
{
	m_guimgr.onTextEntered(evt);
}
