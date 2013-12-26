#ifndef MENU_HPP_INCLUDED
#define MENU_HPP_INCLUDED

#include "ApplicationState.hpp"
#include "Button.hpp"
#include <SFML/Graphics.hpp>

class Menu : public ApplicationState
{
	public:
	Menu(sf::RenderWindow &window, float vratio, float xyratio);
	virtual ~Menu();

	Menu(const Menu &) = delete;
	Menu &operator=(const Menu &) = delete;

	virtual void load();
	virtual void update(float etime);
	virtual void draw();

	virtual void onWindowClosed();
	virtual void onKeyPressed(const sf::Event::KeyEvent &evt);
	virtual void onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt);
	virtual void onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt);
	virtual void onMouseMoved(const sf::Event::MouseMoveEvent &evt);

	private:
	sf::RenderWindow &m_window;
	sf::View m_camera;//View for the drawables that NEED TO BE SCALED (e.g. images), but not the other ones (e.g. fonts)
	float m_vratio;
	float m_xyratio;
	sf::Sprite m_cursor;

	std::vector<Button> m_buttons;
};

#endif // MENU_HPP_INCLUDED
