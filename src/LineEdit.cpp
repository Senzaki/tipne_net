#include "LineEdit.hpp"
#include "ResourceManager.hpp"

static constexpr const unsigned int LINEEDIT_FONT_SIZE = 12;

LineEdit::LineEdit(Widget *parent, float width, unsigned int maxchar, std::function<void(std::string)> callback):
	Widget(parent),
	m_inputison(false),
	m_maxchar(maxchar),
	m_positioncursor(0),
	m_width(width),
	m_func(callback)
{
	m_text.setFont(ResourceManager::getInstance().getFont(ResourceSection::Base, Resource::STANDARD_FONT));
	m_text.setCharacterSize(LINEEDIT_FONT_SIZE);
	m_text.setColor(sf::Color::White);
	m_string = "";
	m_text.setString("T");
	setSize(m_width, getParent()->getSize().y);
	m_cursor.setSize(sf::Vector2f(2, std::round(m_text.getLocalBounds().height + 3)));
	m_cursor.setFillColor(sf::Color::White);
	m_text.setString(m_string);

	onPositionChanged();

	updateCursor();
}

void LineEdit::setMaxChar(unsigned int maxchar)
{
	m_maxchar = maxchar;
}

void LineEdit::setWidth(float width)
{
	m_width = width;
}

void LineEdit::setCallback(std::function<void(std::string)> callback)
{
	m_func = callback;
}

void LineEdit::setString(std::string string)
{
	m_string = sf::String::fromUtf8(string.begin(), string.end());
	m_text.setString(m_string);
	m_positioncursor = m_string.getSize();
	updateCursor();
}

std::string LineEdit::getString() const
{
	return m_string.toAnsiString();
}

void LineEdit::draw(sf::RenderWindow &window)
{
	window.draw(m_text);
	if(m_inputison)
		window.draw(m_cursor);
}

void LineEdit::onPositionChanged()
{
	sf::Vector2f size = getSize();
	sf::Vector2f pos = getParent()->getAbsolutePosition();
	//Put the text right in the middle of the parent widget (coords are integer so that the text does not look blurred)
	if(m_string == "")
		m_text.setString("T"); //so the text has a height (temporary)
	sf::FloatRect textrect = m_text.getLocalBounds();
	m_text.setString(m_string);

	m_text.setPosition(std::round(pos.x + 5), std::round(pos.y + (size.y - textrect.height) / 2.f));

	updateCursor();
}

bool LineEdit::onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt)
{
	m_inputison = mouseIsOn();
	return true;
}

bool LineEdit::onTextEntered(const sf::Event::TextEvent &evt)
{
	if(m_inputison)
	{
		//Check whether the character is valid or not
		if((m_string.getSize() < m_maxchar || m_maxchar == 0)
			&& (evt.unicode > 31 &&(evt.unicode < 127 || evt.unicode > 159)))
		{
			//add the character
			m_string.insert(m_positioncursor, evt.unicode);
			m_positioncursor++;
		}

		m_text.setString(m_string);
	}

	updateCursor();
	return true;
}

bool LineEdit::onKeyPressed(const sf::Event::KeyEvent &evt)
{
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
			if(m_positioncursor < m_string.getSize())
				m_positioncursor++;
			break;

			case sf::Keyboard::BackSpace:
			//erase character
			if(m_positioncursor > 0)
			{
				m_string.erase(m_positioncursor - 1, 1);
				m_positioncursor--;
			}
			m_text.setString(m_string);
			break;

			case sf::Keyboard::Delete:
			//erase character
			if(m_positioncursor < m_string.getSize())
				m_string.erase(m_positioncursor, 1);
			m_text.setString(m_string);
			break;

			default:
			break;
		}

		updateCursor();
	}
	return true;
}

void LineEdit::updateCursor()
{
	if(m_positioncursor == 0)
		m_cursor.setPosition(std::round(getAbsolutePosition().x) + 5, std::round(m_text.getPosition().y));
	else
		m_cursor.setPosition(std::round(m_text.findCharacterPos(m_positioncursor).x), std::round(m_text.getPosition().y));
}
