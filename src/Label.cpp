#include "Label.hpp"
#include "ResourceManager.hpp"

static constexpr const unsigned int LABEL_FONT_SIZE = 12;

Label::Label(Widget *parent, std::string string):
	Widget(parent)
{
	m_text.setFont(ResourceManager::getInstance().getFont(ResourceSection::Base, Resource::STANDARD_FONT));
	m_text.setCharacterSize(LABEL_FONT_SIZE);
	m_text.setColor(sf::Color::White);
	setString(string);
}

void Label::setString(std::string string)
{
	m_text.setString(sf::String::fromUtf8(string.begin(), string.end()));
	updateSize();
}

void Label::draw(sf::RenderWindow &window)
{
	window.draw(m_text);
}

void Label::onPositionChanged()
{
	m_text.setPosition(std::round(getAbsolutePosition().x), std::round(getAbsolutePosition().y));
}

void Label::updateSize()
{
	setSize(m_text.getLocalBounds().width, m_text.getLocalBounds().height + 3);
}
