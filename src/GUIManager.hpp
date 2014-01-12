#ifndef GUIMANAGER_HPP_INCLUDED
#define GUIMANAGER_HPP_INCLUDED

#include <SFML/Graphics.hpp>
#include "Widget.hpp"
#include <stack>

class GUIManager
{
	public:
	GUIManager(sf::RenderWindow &window);
	~GUIManager();

	GUIManager(const GUIManager &) = delete;
	GUIManager &operator=(const GUIManager &) = delete;

	void update(float etime);
	void draw();

	void clear();//deletes all widgets

	void setModalWidget(Widget *widget); //widget = nullptr means no modal widget
	Widget *getModalWidget();
	const Widget *getModalWidget() const;

	Widget *getTopWidget();
	const Widget *getTopWidget() const;

	//These functions return true if the event is consumed
	bool onTextEntered(const sf::Event::TextEvent &evt);
	bool onKeyPressed(const sf::Event::KeyEvent &evt);
	bool onKeyReleased(const sf::Event::KeyEvent &evt);
	bool onMouseWheelMoved(const sf::Event::MouseWheelEvent &evt);
	bool onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt);
	bool onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt);
	void onMouseMoved(const sf::Event::MouseMoveEvent &evt);

	private:
	void updateWidget(Widget *widget, float etime);
	void drawWidget(Widget *widget);

	std::stack<sf::FloatRect> m_drawingzones;

	sf::RenderWindow &m_window;

	Widget *m_topwidget;
	Widget *m_modalwidget;

	float m_winheight;
};

#endif // GUIMANAGER_HPP_INCLUDED
