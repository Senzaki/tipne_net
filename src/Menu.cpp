#include "Menu.hpp"
#include "Application.hpp"
#include "ResourceManager.hpp"

Menu::Menu(sf::RenderWindow &window, float vratio, float xyratio):
	m_window(window),
	m_camera(sf::FloatRect(0.f, 0.f, xyratio * DEFAULT_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT)),
	m_vratio(vratio),
	m_xyratio(xyratio)
{

}

Menu::~Menu()
{
	ResourceManager::getInstance().unloadSection(ResourceSection::Menu);
}

void Menu::load()
{
	ResourceManager &rmgr = ResourceManager::getInstance();
	//Load all textures & fonts
	rmgr.loadSection(ResourceSection::Menu);
	m_cursor.setTexture(rmgr.getTexture(ResourceSection::Base, "cursor.png"));
}

void Menu::update(float etime)
{

}

void Menu::draw()
{
	m_window.clear();
	//Apply the scaling view
	m_window.setView(m_camera);
	m_window.draw(m_cursor);
	m_window.setView(m_window.getDefaultView());
}

void Menu::onWindowClosed()
{
	Application::getInstance().setNextAppState(nullptr);
}

void Menu::onKeyPressed(const sf::Event::KeyEvent &evt)
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

void Menu::onMouseMoved(const sf::Event::MouseMoveEvent &evt)
{
	//Cursor is displayed in camera coords, but mouse coordinates are given in window coordinates
	m_cursor.setPosition(evt.x / m_vratio, evt.y / m_vratio);
}
