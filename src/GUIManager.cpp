#include "GUIManager.hpp"
#include <GL/gl.h>
#include <iostream>

GUIManager::GUIManager(sf::RenderWindow &window):
	m_window(window),
	m_topwidget(nullptr),
	m_modalwidget(nullptr),
	m_winheight(window.getSize().y)
{
	//Create the top widget
	clear();
}

GUIManager::~GUIManager()
{
	//Delete widgets to be removed
	for(Widget *del : m_todelete)
		delete del;
	m_todelete.clear();

	delete m_topwidget;
}

void GUIManager::update(float etime)
{
	//Delete widgets to be removed
	for(Widget *del : m_todelete)
		delete del;
	m_todelete.clear();

	updateWidget(m_topwidget, etime);
}

void GUIManager::updateWidget(Widget *widget, float etime)
{
	//Simple recursive func
	widget->update(etime);
	for(Widget *child : widget->m_children)
		updateWidget(child, etime);
}

void GUIManager::draw()
{
	//Enable scissor test so that widgets don't go out of their boundaries
	glEnable(GL_SCISSOR_TEST);
	drawWidget(m_topwidget);
	glDisable(GL_SCISSOR_TEST);
}

void GUIManager::deleteLater(Widget *widget)
{
#ifndef NDEBUG
	std::cout << "[DEBUG]The parent is set to NULL on purpose. You can safely ignore next warning." << std::endl;
#endif
	widget->setParent(nullptr);
	m_todelete.push_back(widget);
}

void GUIManager::clear()
{
	if(m_topwidget)
		deleteLater(m_topwidget);
	m_topwidget = new Widget(nullptr);
	m_topwidget->setSize(m_window.getSize().x, m_window.getSize().y);
}

void GUIManager::drawWidget(Widget *widget)
{
	sf::Vector2f pos = widget->getAbsolutePosition();
	sf::Vector2f size = widget->getSize();

	//Compute the intersection of the current zone and the parent zone if required
	if(!m_drawingzones.empty())
	{
		const sf::FloatRect &oldzone = m_drawingzones.top();
		if(pos.x < oldzone.left)
			pos.x = oldzone.left;
		float maxwidth = oldzone.left + oldzone.width - pos.x; //Max right border - left border
		if(size.x > maxwidth)
			size.x = maxwidth;
		if(pos.y < oldzone.top)
			pos.y = oldzone.top;
		float maxheight = oldzone.top + oldzone.height - pos.y;
		if(size.y > maxheight)
			size.y = maxheight;
	}

	//Push the drawing zone
	m_drawingzones.emplace(pos, size);
	//Update the scissor. glScissor uses coords from the lower left corner
	glScissor(pos.x, m_winheight - pos.y - size.y, size.x, size.y);

	widget->draw(m_window);
	for(Widget *child : widget->m_children)
		drawWidget(child);

	//Pop the drawing zone
	m_drawingzones.pop();
}

void GUIManager::setModalWidget(Widget *widget)
{
	m_modalwidget = widget;
}

Widget *GUIManager::getModalWidget()
{
	return m_modalwidget;
}

const Widget *GUIManager::getModalWidget() const
{
	return m_modalwidget;
}

Widget *GUIManager::getTopWidget()
{
	return m_topwidget;
}

const Widget *GUIManager::getTopWidget() const
{
	return m_topwidget;
}

bool GUIManager::onTextEntered(const sf::Event::TextEvent &evt)
{
	return m_topwidget->onTextEntered(evt);
}

bool GUIManager::onKeyPressed(const sf::Event::KeyEvent &evt)
{
	return m_topwidget->onKeyPressed(evt);
}

bool GUIManager::onKeyReleased(const sf::Event::KeyEvent &evt)
{
	return m_topwidget->onKeyReleased(evt);
}

bool GUIManager::onMouseWheelMoved(const sf::Event::MouseWheelEvent &evt)
{
	return m_topwidget->onMouseWheelMoved(evt);
}

bool GUIManager::onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt)
{
	return m_topwidget->onMouseButtonPressed(evt);
}

bool GUIManager::onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt)
{
	return m_topwidget->onMouseButtonReleased(evt);
}

void GUIManager::onMouseMoved(const sf::Event::MouseMoveEvent &evt)
{
	m_topwidget->onMouseMoved(evt);
}
