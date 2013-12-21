#include "Menu.hpp"
#include "Application.hpp"

Menu::Menu(sf::RenderWindow &window):
	m_window(window)
{

}

Menu::~Menu()
{

}

void Menu::update(float etime)
{

}

void Menu::draw()
{

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
