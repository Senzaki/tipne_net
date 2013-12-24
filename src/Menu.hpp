#ifndef MENU_HPP_INCLUDED
#define MENU_HPP_INCLUDED

#include "ApplicationState.hpp"
#include <SFML/Graphics.hpp>

class Menu : public ApplicationState
{
	public:
	Menu(sf::RenderWindow &window);
	virtual ~Menu();

	Menu(const Menu &) = delete;
	Menu &operator=(const Menu &) = delete;

	virtual void load();
	virtual void update(float etime);
	virtual void draw();

	virtual void onWindowClosed();
	virtual void onKeyPressed(const sf::Event::KeyEvent &evt);
	virtual void onMouseMoved(const sf::Event::MouseMoveEvent &evt);

	private:
	sf::RenderWindow &m_window;

	sf::Sprite m_cursor;
};

#endif // MENU_HPP_INCLUDED
