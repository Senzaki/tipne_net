#ifndef WIDGET_HPP_INCLUDED
#define WIDGET_HPP_INCLUDED

#include <list>
#include <SFML/Graphics.hpp>

class Widget
{
	public:
	Widget(Widget *parent = nullptr);
	virtual ~Widget();

	Widget(const Widget &) = delete;
	Widget &operator=(const Widget &) = delete;

	virtual void update(float etime) { }
	virtual void draw(sf::RenderWindow &window) { }

	void setPosition(float x, float y);
	void setPosition(const sf::Vector2f &pos);
	sf::Vector2f getPosition() const;
	sf::Vector2f getAbsolutePosition() const;

	sf::Vector2f getSize() const;

	void setParent(Widget *parent);
	Widget *getParent() const;

	protected:
	//These functions return true if the event is consumed
	virtual bool onTextEntered(const sf::Event::TextEvent &evt);
	virtual bool onKeyPressed(const sf::Event::KeyEvent &evt);
	virtual bool onKeyReleased(const sf::Event::KeyEvent &evt);
	virtual bool onMouseWheelMoved(const sf::Event::MouseWheelEvent &evt);
	virtual bool onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt);
	virtual bool onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt);
	virtual void onMouseMoved(const sf::Event::MouseMoveEvent &evt);
	virtual void onMouseEntered(const sf::Event::MouseMoveEvent &evt); //A duplicate event mouseMoved will be sent
	virtual void onMouseLeft();

	void setSize(const sf::Vector2f &size);
	void setSize(float w, float h);

	const std::list<Widget*> &getChildren() const;

	bool mouseIsOn() const;

	virtual void onPositionChanged() { }

	private:
	sf::Vector2f m_pos;
	sf::Vector2f m_abspos;
	sf::Vector2f m_size;

	Widget *m_parent;

	void addChild(Widget *child);
	void removeChild(Widget *child);
	std::list<Widget*> m_children;

	bool m_mouseison;

	friend class GUIManager;
};

#endif // WIDGET_HPP_INCLUDED
