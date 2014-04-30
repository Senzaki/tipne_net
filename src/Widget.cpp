#include "Widget.hpp"
#include <iostream>

Widget::Widget(Widget *parent):
	m_parent(parent),
	m_mouseison(false)
{
	if(parent)
		parent->addChild(this);
	setPosition(0.f, 0.f);
}

Widget::~Widget()
{
	//Delete children
	for(Widget *child : m_children)
	{
		child->m_parent = nullptr;
		delete child;
	}

	if(m_parent)
		m_parent->removeChild(this);
}

void Widget::setPosition(float x, float y)
{
	m_pos.x = x;
	m_pos.y = y;

	if(m_parent)
		m_abspos = m_parent->m_abspos + m_pos;
	else
		m_abspos = m_pos;

	//Notify event
	onPositionChanged();

	//Update children
	for(Widget *child : m_children)
		child->setPosition(child->getPosition());
}

void Widget::setPosition(const sf::Vector2f &pos)
{
	setPosition(pos.x, pos.y);
}

sf::Vector2f Widget::getPosition() const
{
	return m_pos;
}

sf::Vector2f Widget::getAbsolutePosition() const
{
	return m_abspos;
}

void Widget::setSize(const sf::Vector2f &size)
{
	m_size = size;
	onPositionChanged();
}

void Widget::setSize(float w, float h)
{
	m_size.x = w;
	m_size.y = h;
	onPositionChanged();
}

sf::Vector2f Widget::getSize() const
{
	return m_size;
}

void Widget::setParent(Widget *parent)
{
	if(m_parent)
		m_parent->removeChild(this);
	m_parent = parent;
	//If there's a new parent, notify it that we're its child
	if(parent)
		parent->addChild(this);
#ifndef NDEBUG
	else
		std::cout << "[DEBUG]Warning : Setting a widget's parent to NULL may result in a memory leak. Be sure to free it properly." << std::endl;
#endif
}

Widget *Widget::getParent() const
{
	return m_parent;
}

const std::list<Widget*> &Widget::getChildren() const
{
	return m_children;
}

bool Widget::mouseIsOn() const
{
	return m_mouseison;
}

bool Widget::onTextEntered(const sf::Event::TextEvent &evt)
{
	//Transmit to children
	for(Widget *child : m_children)
	{
		if(child->onTextEntered(evt))
			return true;
	}

	return false;
}

bool Widget::onKeyPressed(const sf::Event::KeyEvent &evt)
{
	//Transmit to children
	for(Widget *child : m_children)
	{
		if(child->onKeyPressed(evt))
			return true;
	}

	return false;
}

bool Widget::onKeyReleased(const sf::Event::KeyEvent &evt)
{
	//Transmit to children
	for(Widget *child : m_children)
	{
		if(child->onKeyReleased(evt))
			return true;
	}

	return false;
}

bool Widget::onMouseWheelMoved(const sf::Event::MouseWheelEvent &evt)
{
	//Transmit to children if they contain the mouse position
	for(Widget *child : m_children)
	{
		sf::FloatRect childrect(child->getAbsolutePosition(), child->getSize());
		if(childrect.contains(evt.x, evt.y))
		{
			if(child->onMouseWheelMoved(evt))
				return true;
		}
	}

	return false;
}

bool Widget::onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt)
{
	//Transmit to children if they contain the mouse position
	for(Widget *child : m_children)
	{
		if(child->onMouseButtonPressed(evt))
			return true;
	}

	return false;
}

bool Widget::onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt)
{
	//Transmit to children if they contain the mouse position
	for(Widget *child : m_children)
	{
		if(child->onMouseButtonReleased(evt))
			return true;
	}

	return false;
}

void Widget::onMouseMoved(const sf::Event::MouseMoveEvent &evt)
{
	//Transmit to children if they contain the mouse position
	for(Widget *child : m_children)
	{
		sf::FloatRect childrect(child->getAbsolutePosition(), child->getSize());
		if(childrect.contains(evt.x, evt.y))
		{
			//If the mouse just entered in the child, notify it
			if(!child->m_mouseison)
				child->onMouseEntered(evt);

			child->onMouseMoved(evt);
		}
		else
		{
			//If the mouse just left from the child, notify it
			if(child->m_mouseison)
				child->onMouseLeft();
		}
	}
}

void Widget::onMouseEntered(const sf::Event::MouseMoveEvent &evt)
{
	m_mouseison = true;
	//Transmit to children if they contain the mouse position
	for(Widget *child : m_children)
	{
		sf::FloatRect childrect(child->getAbsolutePosition(), child->getSize());
		if(childrect.contains(evt.x, evt.y))
			child->onMouseEntered(evt);
	}
}

void Widget::onMouseLeft()
{
	m_mouseison = false;

	//Transmit to children if the mouse was over them
	for(Widget *child : m_children)
	{
		if(child->m_mouseison)
			child->onMouseLeft();
	}
}

void Widget::addChild(Widget *child)
{
	m_children.push_back(child);
}

void Widget::removeChild(Widget *child)
{
	m_children.remove(child);
}
