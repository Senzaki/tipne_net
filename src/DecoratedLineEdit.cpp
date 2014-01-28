#include "DecoratedLineEdit.hpp"
#include "ResourceManager.hpp"
#include <iostream>

static constexpr const unsigned int LINEEDIT_LEFT_BORDER_WIDTH = 5;
static constexpr const unsigned int LINEEDIT_RIGHT_BORDER_WIDTH = 5;

DecoratedLineEdit::DecoratedLineEdit(Widget *parent, float width, unsigned int maxchar, std::function<void(std::string)> callback):
	Widget(parent),
	m_width(width),
	m_background(sf::TrianglesStrip, 8)
{
	m_rdstates.texture = &ResourceManager::getInstance().getTexture(ResourceSection::Base, Resource::LINEEDIT_STANDARD_TEX);

	//Setup texture coords (& positions for the first 4 points, because le position of the left border never changes)
	float texheight = m_rdstates.texture->getSize().y;
	m_background[0].position = m_background[0].texCoords = sf::Vector2f(0.f, texheight);
	m_background[1].position = m_background[1].texCoords = sf::Vector2f(0.f, 0.f);
	m_background[2].position = m_background[2].texCoords = sf::Vector2f(LINEEDIT_LEFT_BORDER_WIDTH, texheight);
	m_background[3].position = m_background[3].texCoords = sf::Vector2f(LINEEDIT_LEFT_BORDER_WIDTH, 0.f);
	m_background[4].texCoords = sf::Vector2f(LINEEDIT_LEFT_BORDER_WIDTH + 1, texheight);
	m_background[5].texCoords = sf::Vector2f(LINEEDIT_LEFT_BORDER_WIDTH + 1, 0.f);
	m_background[6].texCoords = sf::Vector2f(LINEEDIT_LEFT_BORDER_WIDTH + 1 + LINEEDIT_RIGHT_BORDER_WIDTH, texheight);
	m_background[7].texCoords = sf::Vector2f(LINEEDIT_LEFT_BORDER_WIDTH + 1 + LINEEDIT_RIGHT_BORDER_WIDTH, 0.f);

	updateSize();

	m_lineedit = new LineEdit(this, width - LINEEDIT_RIGHT_BORDER_WIDTH, maxchar, callback);
}

DecoratedLineEdit::~DecoratedLineEdit()
{
	m_lineedit = nullptr;
	delete m_lineedit;
}

void DecoratedLineEdit::setString(std::string string)
{
	m_lineedit->setString(string);
}

std::string DecoratedLineEdit::getString() const
{
	return m_lineedit->getString();
}

void DecoratedLineEdit::draw(sf::RenderWindow &window)
{
	window.draw(m_background, sf::RenderStates(m_rdstates));
}

void DecoratedLineEdit::onPositionChanged()
{
	//Top left corner
	m_rdstates.transform = sf::Transform::Identity;
	m_rdstates.transform.translate(getAbsolutePosition());
}

void DecoratedLineEdit::updateSize()
{
	float width = m_width - LINEEDIT_RIGHT_BORDER_WIDTH;
	float texheight = m_rdstates.texture->getSize().y;
	//Setup points position
	m_background[4].position = sf::Vector2f(width, texheight);
	m_background[5].position = sf::Vector2f(width, 0.f);
	m_background[6].position = sf::Vector2f(width + LINEEDIT_RIGHT_BORDER_WIDTH, texheight);
	m_background[7].position = sf::Vector2f(width + LINEEDIT_RIGHT_BORDER_WIDTH, 0.f);

	onPositionChanged();

	setSize(width + LINEEDIT_RIGHT_BORDER_WIDTH, texheight);
}

