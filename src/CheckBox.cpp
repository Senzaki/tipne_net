#include "CheckBox.hpp"
#include "ResourceManager.hpp"


CheckBox::CheckBox(Widget *parent , std::function<void()> callbackcheck , std::function<void()> callbackuncheck):
	Widget(parent),
	m_isChecked(false),
	m_funcCheck(callbackcheck),
	m_funcUncheck(callbackuncheck)
{
	//set texture standard
	m_background.setTexture(ResourceManager::getInstance().getTexture(ResourceSection::Base , Resource::CHECKBOX_UNCHECKED_TEX));
	setSize(sf::Vector2f(m_background.getGlobalBounds().width , m_background.getGlobalBounds().height));
}

void CheckBox::setCallbackCheck(std::function<void()> callback)
{
	m_funcCheck = callback;
}

void CheckBox::setCallbackUncheck(std::function<void()> callback)
{
	m_funcUncheck = callback;
}

const bool &CheckBox::isChecked()
{
	return m_isChecked;
}

void CheckBox::draw(sf::RenderWindow &window)
{
	window.draw(m_background);
}

void CheckBox::onPositionChanged()
{
	m_background.setPosition(getAbsolutePosition());
}

bool CheckBox::onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt)
{
	Widget::onMouseButtonReleased(evt);
	if(m_background.getGlobalBounds().contains(evt.x , evt.y))
	{
		m_isChecked = !m_isChecked;
		//call the appropriate function
		if(m_isChecked && m_funcCheck)
			m_funcCheck();
		else if(!m_isChecked && m_funcUncheck)
			m_funcUncheck();
		//set the appropriate texture
		if(m_isChecked)
			m_background.setTexture(ResourceManager::getInstance().getTexture(ResourceSection::Base , Resource::CHECKBOX_CHECKED_TEX));
		else
			m_background.setTexture(ResourceManager::getInstance().getTexture(ResourceSection::Base , Resource::CHECKBOX_UNCHECKED_TEX));
	}
	return true;
}
