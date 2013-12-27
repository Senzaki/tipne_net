#include "Button.hpp"
#include "ResourceManager.hpp"

static constexpr const char *BUTTON_DEFAULT_FONT = "DejaVuSansMono.ttf";
static constexpr const unsigned int BUTTON_DEFAULT_FONT_SIZE = 12;
static constexpr const char *BUTTON_DEFAULT_STANDARD_TEX = "btn_bg.png";
static constexpr const char *BUTTON_DEFAULT_HIGHLIGHTED_TEX = "btn_hl.png";
static constexpr const char *BUTTON_DEFAULT_CLICKED_TEX = "btn_cl.png";

Button::Button(std::function<void()> callback):
	m_stdtex(nullptr),
	m_hltex(nullptr),
	m_cltex(nullptr),
	m_highlighted(false),
	m_clicked(false),
	m_func(callback)
{
	m_text.setCharacterSize(BUTTON_DEFAULT_FONT_SIZE);
	resetResources();
}

Button::Button(const std::string &text, std::function<void()> callback):
	Button(callback)
{
	setText(text);
}

Button::Button(const std::string &text, const sf::Font &font, const sf::Texture &normaltex, const sf::Texture &hltex, const sf::Texture &clicktex, std::function<void()> callback):
	m_stdtex(&normaltex),
	m_hltex(&hltex),
	m_cltex(&clicktex),
	m_text(text, font, BUTTON_DEFAULT_FONT_SIZE),
	m_highlighted(false),
	m_clicked(false),
	m_func(callback)
{
	updateBackground();
}

void Button::resetResources()
{
	//Use all default resources. The "Base" resources section is supposed to be loaded whenever a button is created
	ResourceManager &rsmgr = ResourceManager::getInstance();
	m_text.setFont(rsmgr.getFont(ResourceSection::Base, BUTTON_DEFAULT_FONT));
	m_stdtex = &rsmgr.getTexture(ResourceSection::Base, BUTTON_DEFAULT_STANDARD_TEX);
	m_stdcol = sf::Color::White;
	m_hltex = &rsmgr.getTexture(ResourceSection::Base, BUTTON_DEFAULT_HIGHLIGHTED_TEX);
	m_hlcol = sf::Color::White;
	m_cltex = &rsmgr.getTexture(ResourceSection::Base, BUTTON_DEFAULT_CLICKED_TEX);
	m_clcol = sf::Color::Black;
	updateBackground();
}

void Button::setNormalTexture(const sf::Texture &tex)
{
	m_stdtex = &tex;
	updateBackground();
}

void Button::setNormalCharColor(const sf::Color &color)
{
	m_stdcol = color;
	updateBackground();
}

void Button::setHighlightTexture(const sf::Texture &tex)
{
	m_hltex = &tex;
	updateBackground();
}

void Button::setHighlightCharColor(const sf::Color &color)
{
	m_hlcol = color;
	updateBackground();
}

void Button::setClickedTexture(const sf::Texture &tex)
{
	m_cltex = &tex;
	updateBackground();
}

void Button::setClickedCharColor(const sf::Color &color)
{
	m_clcol = color;
	updateBackground();
}

void Button::setFont(const sf::Font &font)
{
	m_text.setFont(font);
}

void Button::setPosition(float x, float y)
{
	//Top left corner
	m_background.setPosition(x, y);
	//Get new rectangle
	m_rect = m_background.getGlobalBounds();
	//Put the text right in the middle of the button
	sf::FloatRect textrect = m_text.getLocalBounds();
	m_text.setPosition(m_rect.left + (m_rect.width - textrect.width) / 2.f, m_rect.top + (m_rect.height - textrect.height) / 2.f);
}

const sf::FloatRect &Button::getGlobalBounds() const
{
	return m_rect;
}

void Button::setText(const std::string &text)
{
	//Tell SFML the string is UTF-8
	m_text.setString(sf::String::fromUtf8(text.begin(), text.end()));
	//Update layout
	setPosition(m_rect.left, m_rect.top);
}

void Button::setCallback(std::function<void()> callback)
{
	m_func = callback;
}

void Button::onMouseButtonPressed(const sf::Vector2f &coords)
{
	bool oldhl = m_highlighted;
	bool oldcl = m_clicked;
	//Does the button contain the coords ?
	m_highlighted = m_rect.contains(coords);
	if(m_highlighted)
		m_clicked = true;
	//Update the appearance (only if required)
	if(oldhl != m_highlighted || oldcl != m_clicked)
		updateBackground();
}

void Button::onMouseButtonReleased(const sf::Vector2f &coords)
{
	bool oldhl = m_highlighted;
	bool oldcl = m_clicked;
	//Does the button contain the coords ?
	m_highlighted = m_rect.contains(coords);
	//Call the callback if clicked (and valid)
	if(m_highlighted && m_clicked && m_func)
		m_func();
	m_clicked = false;
	//Update the appearance (only if required)
	if(oldhl != m_highlighted || oldcl != m_clicked)
		updateBackground();
}

void Button::onMouseMoved(const sf::Vector2f &coords)
{
	bool oldhl = m_highlighted;
	//Does the button contain the coords ?
	m_highlighted = m_rect.contains(coords);
	//Update the appearance (only if required)
	if(oldhl != m_highlighted)
		updateBackground();
}

void Button::draw(sf::RenderWindow &window)
{
	window.draw(m_background);
	window.draw(m_text);
}

void Button::updateBackground()
{
	//Set the appropriate texture
	if(m_highlighted)
	{
		if(m_clicked)
		{
			m_background.setTexture(*m_cltex, true);
			m_text.setColor(m_clcol);
		}
		else
		{
			m_background.setTexture(*m_hltex, true);
			m_text.setColor(m_hlcol);
		}
	}
	else
	{
		m_background.setTexture(*m_stdtex, true);
		m_text.setColor(m_stdcol);
	}
	//Update the layout
	setPosition(m_rect.left, m_rect.top);
}
