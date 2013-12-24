#include "Menu.hpp"
#include "Application.hpp"
#include "ResourceManager.hpp"

Menu::Menu(sf::RenderWindow &window):
	m_window(window)
{

}

Menu::~Menu()
{
	ResourceManager::getInstance().unloadSection(ResourceSection::Menu);
}

void Menu::load()
{
	ResourceManager &rmgr = ResourceManager::getInstance();
	rmgr.loadSection(ResourceSection::Menu);
	m_cursor.setTexture(rmgr.getTexture(ResourceSection::Base, "cursor.png"));
}

void Menu::update(float etime)
{

}

void Menu::draw()
{
	m_window.clear();
	m_window.draw(m_cursor);
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
	m_cursor.setPosition(evt.x, evt.y);
}
