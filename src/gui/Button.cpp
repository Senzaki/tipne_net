#include "Button.hpp"
#include "ResourceManager.hpp"

static constexpr const unsigned int BUTTON_FONT_SIZE = 12;
static constexpr const unsigned int BUTTON_LEFT_BORDER_WIDTH = 54;
static constexpr const unsigned int BUTTON_RIGHT_BORDER_WIDTH = 54;

Button::Button(Widget *parent, std::function<void()> callback):
	Widget(parent),
	m_background(sf::TrianglesStrip, 8),
	m_minw(0.f),
	m_clicked(false),
	m_func(callback)
{
	m_text.setCharacterSize(BUTTON_FONT_SIZE);
	m_text.setFont(ResourceManager::getInstance().getFont(ResourceSection::Base, Resource::STANDARD_FONT));

	//Set "standard" appearance
	m_rdstates.texture = &ResourceManager::getInstance().getTexture(ResourceSection::Base, Resource::BUTTON_STANDARD_TEX);
	m_text.setColor(sf::Color::White);

	//Setup texture coords (& positions for the first 4 points, because le position of the left border never changes)
	float texheight = m_rdstates.texture->getSize().y;
	m_background[0].position = m_background[0].texCoords = sf::Vector2f(0.f, texheight);
	m_background[1].position = m_background[1].texCoords = sf::Vector2f(0.f, 0.f);
	m_background[2].position = m_background[2].texCoords = sf::Vector2f(BUTTON_LEFT_BORDER_WIDTH, texheight);
	m_background[3].position = m_background[3].texCoords = sf::Vector2f(BUTTON_LEFT_BORDER_WIDTH, 0.f);
	m_background[4].texCoords = sf::Vector2f(BUTTON_LEFT_BORDER_WIDTH + 1, texheight);
	m_background[5].texCoords = sf::Vector2f(BUTTON_LEFT_BORDER_WIDTH + 1, 0.f);
	m_background[6].texCoords = sf::Vector2f(BUTTON_LEFT_BORDER_WIDTH + 1 + BUTTON_RIGHT_BORDER_WIDTH, texheight);
	m_background[7].texCoords = sf::Vector2f(BUTTON_LEFT_BORDER_WIDTH + 1 + BUTTON_RIGHT_BORDER_WIDTH, 0.f);

	updateSize();
}

Button::Button(Widget *parent, const std::string &text, std::function<void()> callback):
	Button(parent, callback)
{
	setText(text);
}

Button::~Button()
{

}

void Button::onPositionChanged()
{
	sf::Vector2f pos = getAbsolutePosition();
	sf::Vector2f size = getSize();
	//Top left corner
	m_rdstates.transform = sf::Transform::Identity;
	m_rdstates.transform.translate(pos);
	//Put the text right in the middle of the button (coords are integer so that the text does not look blurred)
	sf::FloatRect textrect = m_text.getLocalBounds();
	m_text.setPosition(std::round(pos.x + (size.x - textrect.width) / 2.f), std::round(pos.y + (size.y - textrect.height) / 2.f));
}

void Button::setText(const std::string &text)
{
	//Tell SFML the string is UTF-8
	m_text.setString(sf::String::fromUtf8(text.begin(), text.end()));
	//Update layout
	updateSize();
}

void Button::setCallback(std::function<void()> callback)
{
	m_func = callback;
}

void Button::setMinimumWidth(float minwidth)
{
	m_minw = minwidth;
	updateSize();
}

void Button::draw(sf::RenderWindow &window)
{
	window.draw(m_background, sf::RenderStates(m_rdstates));
	window.draw(m_text);
}

bool Button::onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt)
{
	sf::FloatRect bounds(getAbsolutePosition(), getSize());
	if(bounds.contains(evt.x, evt.y))
	{
		Widget::onMouseButtonPressed(evt);
		m_clicked = true;
		//Set "clicked" appearance
		m_rdstates.texture = &ResourceManager::getInstance().getTexture(ResourceSection::Base, Resource::BUTTON_CLICKED_TEX);
		m_text.setColor(sf::Color::White);

		return true;
	}

	return false;
}

bool Button::onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt)
{
	sf::FloatRect bounds(getAbsolutePosition(), getSize());
	if(bounds.contains(evt.x, evt.y))
	{
		Widget::onMouseButtonReleased(evt);
		if(m_clicked && m_func)
			m_func();
		m_clicked = false;
		//Set "highlighted" appearance
		m_rdstates.texture = &ResourceManager::getInstance().getTexture(ResourceSection::Base, Resource::BUTTON_HIGHLIGHTED_TEX);
		m_text.setColor(sf::Color::White);

		return true;
	}
	return false;
}

void Button::onMouseEntered(const sf::Event::MouseMoveEvent &evt)
{
	Widget::onMouseEntered(evt);
	//Set "highlighted" appearance
	m_rdstates.texture = &ResourceManager::getInstance().getTexture(ResourceSection::Base, Resource::BUTTON_HIGHLIGHTED_TEX);
	m_text.setColor(sf::Color::White);
}

void Button::onMouseLeft()
{
	Widget::onMouseLeft();
	m_clicked = false;
	//Set "standard" appearance
	m_rdstates.texture = &ResourceManager::getInstance().getTexture(ResourceSection::Base, Resource::BUTTON_STANDARD_TEX);
	m_text.setColor(sf::Color::White);
}

void Button::updateSize()
{
	float width = m_text.getLocalBounds().width + BUTTON_LEFT_BORDER_WIDTH;
	if(width < m_minw - BUTTON_RIGHT_BORDER_WIDTH)
		width = m_minw - BUTTON_RIGHT_BORDER_WIDTH;
	float texheight = m_rdstates.texture->getSize().y;
	//Setup points position
	m_background[4].position = sf::Vector2f(width, texheight);
	m_background[5].position = sf::Vector2f(width, 0.f);
	m_background[6].position = sf::Vector2f(width + BUTTON_RIGHT_BORDER_WIDTH, texheight);
	m_background[7].position = sf::Vector2f(width + BUTTON_RIGHT_BORDER_WIDTH, 0.f);

	onPositionChanged();

	setSize(width + BUTTON_RIGHT_BORDER_WIDTH, texheight);
}
