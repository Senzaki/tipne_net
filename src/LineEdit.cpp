#include "LineEdit.hpp"
#include "ResourceManager.hpp"

static constexpr const unsigned int LINEEDIT_FONT_SIZE = 12;
static constexpr const unsigned int LINEEDIT_REPOS = 5;
static constexpr const unsigned int LINEEDIT_HEIGHT = 15;

LineEdit::LineEdit(Widget *parent, float width, std::function<void(std::string)> callback):
	Widget(parent),
	m_inputison(false),
	m_maxchar(0),
	m_positioncursor(0),
	m_width(width),
	m_func(callback)
{
	m_text.setFont(ResourceManager::getInstance().getFont(ResourceSection::Base, Resource::STANDARD_FONT));
	m_text.setCharacterSize(LINEEDIT_FONT_SIZE);
	m_text.setColor(sf::Color::White);
	setSize(m_width, LINEEDIT_HEIGHT);
	m_cursor.setSize(sf::Vector2f(2, LINEEDIT_HEIGHT));
	m_cursor.setFillColor(sf::Color::White);

	onPositionChanged();

	updateCursor();
}

LineEdit::~LineEdit(){}

void LineEdit::setMaxChar(unsigned int maxchar)
{
	m_maxchar = maxchar;
}

void LineEdit::setWidth(float width)
{
	m_width = width;
	setSize(m_width, getParent()->getSize().y);
}

void LineEdit::setCallback(std::function<void(std::string)> callback)
{
	m_func = callback;
}

void LineEdit::setString(const std::string &string)
{
	m_text.setString(string);
	m_positioncursor = string.size();
	updateCursor();
}

const std::string &LineEdit::getString() const
{
	return std::string((char*)m_text.getString().toUtf8().c_str());
}

void LineEdit::draw(sf::RenderWindow &window)
{
	window.draw(m_text);
	if(m_inputison)
		window.draw(m_cursor);
}

void LineEdit::onPositionChanged()
{
	m_text.setPosition(getAbsolutePosition());
	updateCursor();
}

bool LineEdit::onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt)
{
	sf::FloatRect bounds(getAbsolutePosition(), getSize());
	m_inputison = bounds.contains(evt.x, evt.y);
	if(m_inputison)
		Widget::onMouseButtonPressed(evt);
	return m_inputison;
}

bool LineEdit::onTextEntered(const sf::Event::TextEvent &evt)
{
	Widget::onTextEntered(evt);
	if(m_inputison)
	{
		//Check whether the character is valid or not
		if((m_text.getString().getSize() < m_maxchar || m_maxchar == 0)
			&& (evt.unicode > 31 &&(evt.unicode < 127 || evt.unicode > 159)))
		{
			//add the character
			sf::String temp(m_text.getString());
			temp.insert(m_positioncursor, evt.unicode);
			m_text.setString(temp);
			m_positioncursor++;
		}

		updateCursor();
		return true;
	}
	return false;
}

bool LineEdit::onKeyPressed(const sf::Event::KeyEvent &evt)
{
	Widget::onKeyPressed(evt);
	if(m_inputison)
	{
		switch(evt.code)
		{
			case sf::Keyboard::Return:
				if(m_func)
					m_func(getString());
				m_inputison = false;
				break;

			case sf::Keyboard::Left:
				if(m_positioncursor > 0)
					m_positioncursor--;
				break;

			case sf::Keyboard::Right:
				if(m_positioncursor < m_text.getString().getSize())
					m_positioncursor++;
				break;

			case sf::Keyboard::BackSpace:
				//erase character
				if(m_positioncursor > 0)
				{
					sf::String temp(m_text.getString());
					temp.erase(m_positioncursor - 1, 1);
					m_text.setString(temp);
					m_positioncursor--;
				}
				break;

			case sf::Keyboard::Delete:
				//erase character
				if(m_positioncursor < m_text.getString().getSize())
				{
					sf::String temp(m_text.getString());
					temp.erase(m_positioncursor, 1);
					m_text.setString(temp);
				}
				break;

			default:
				break;
		}

		updateCursor();
		return true;
	}
	return false;
}

void LineEdit::updateCursor()
{
	//Put the cursor after the adequate character
	sf::Vector2f cursorpos(std::round(m_text.findCharacterPos(m_positioncursor).x), std::round(m_text.getPosition().y));
	m_cursor.setPosition(cursorpos);

	//If the cursor is outside of the widget, replace the text
	if(cursorpos.x <= getAbsolutePosition().x || cursorpos.x >= getAbsolutePosition().x + getSize().x - m_cursor.getLocalBounds().width)
	{
		sf::Vector2f pos = m_text.getPosition();
		sf::Vector2f size = getSize();
		sf::Vector2f newpos;
		//Determine the x of the new pos of the text
		if(cursorpos.x <= getAbsolutePosition().x)
		{
			newpos.x = pos.x + LINEEDIT_REPOS;
		}
		if(cursorpos.x - size.x + m_cursor.getLocalBounds().width >= getAbsolutePosition().x)
		{
			newpos.x = pos.x - LINEEDIT_REPOS;
		}
		newpos.y = m_text.getPosition().y;
		m_text.setPosition(newpos);
		//reupdate cursor
		updateCursor();
	}
}
