#ifndef APPLICATIONSTATE_HPP_INCLUDED
#define APPLICATIONSTATE_HPP_INCLUDED

#include <SFML/Window.hpp>

class ApplicationState
{
	public:
	virtual ~ApplicationState() { }

	virtual void update(float etime) = 0;
	virtual void draw() = 0;

	virtual void onWindowClosed() { }
	virtual void onTextEntered(const sf::Event::TextEvent &evt) { }
	virtual void onKeyPressed(const sf::Event::KeyEvent &evt) { }
	virtual void onKeyReleased(const sf::Event::KeyEvent &evt) { }
	virtual void onMouseWheelMoved(const sf::Event::MouseWheelEvent &evt) { }
	virtual void onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt) { }
	virtual void onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt) { }
	virtual void onMouseMoved(const sf::Event::MouseMoveEvent &evt) { }
};

#endif // APPLICATIONSTATE_HPP_INCLUDED
