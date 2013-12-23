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

	private:
	sf::RenderWindow &m_window;
};

#endif // MENU_HPP_INCLUDED
