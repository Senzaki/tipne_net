#include "CheckBox.hpp"
#include "ResourceManager.hpp"


CheckBox::CheckBox(Widget *parent , std::function<void()> callbackcheck, std::function<void()> callbackuncheck):
	Widget(parent),
	m_checked(false),
	m_funccheck(callbackcheck),
	m_funcuncheck(callbackuncheck)
{
	//set standard texture
	m_background.setTexture(ResourceManager::getInstance().getTexture(ResourceSection::Base, Resource::CHECKBOX_UNCHECKED_TEX));
	setSize(sf::Vector2f(m_background.getLocalBounds().width, m_background.getLocalBounds().height));
}

CheckBox::~CheckBox(){}

void CheckBox::setCheckCallback(std::function<void()> callback)
{
	m_funccheck = callback;
}

void CheckBox::setUncheckCallback(std::function<void()> callback)
{
	m_funcuncheck = callback;
}

void CheckBox::setChecked(bool checked)
{
	m_checked = checked;
	//set the appropriate texture
	if(m_checked)
		m_background.setTexture(ResourceManager::getInstance().getTexture(ResourceSection::Base, Resource::CHECKBOX_CHECKED_TEX));
	else
		m_background.setTexture(ResourceManager::getInstance().getTexture(ResourceSection::Base, Resource::CHECKBOX_UNCHECKED_TEX));
}

bool CheckBox::isChecked() const
{
	return m_checked;
}

void CheckBox::draw(sf::RenderWindow &window)
{
	window.draw(m_background);
}

void CheckBox::onPositionChanged()
{
	m_background.setPosition(getAbsolutePosition());
}

bool CheckBox::onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt)
{
	sf::FloatRect bounds(getAbsolutePosition(), getSize());
	if(bounds.contains(evt.x, evt.y))
	{
		Widget::onMouseButtonPressed(evt);

		return true;
	}
	return false;
}

bool CheckBox::onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt)
{
	sf::FloatRect bounds(getAbsolutePosition(), getSize());
	if(bounds.contains(evt.x, evt.y))
	{
		Widget::onMouseButtonReleased(evt);

		m_checked = !m_checked;
		//call the appropriate function
		if(m_checked && m_funccheck)
			m_funccheck();
		else if(!m_checked && m_funcuncheck)
			m_funcuncheck();
		//set the appropriate texture
		if(m_checked)
			m_background.setTexture(ResourceManager::getInstance().getTexture(ResourceSection::Base, Resource::CHECKBOX_CHECKED_TEX));
		else
			m_background.setTexture(ResourceManager::getInstance().getTexture(ResourceSection::Base, Resource::CHECKBOX_UNCHECKED_TEX));

		return true;
	}
	return false;
}
