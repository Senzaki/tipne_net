#include "Button.hpp"
#include "ResourceManager.hpp"

Button::Button() :
	m_text() , m_position(sf::Vector2f(0 , 0))
{
	ResourceManager &rmgr = ResourceManager::getInstance();
	//Load all textures & fonts
	rmgr.loadSection(ResourceSection::Menu);
	m_text.setFont(rmgr.getFont(ResourceSection::Base , "ASafePlacetoFall.ttf"));
	m_text.setPosition(m_position);
	m_text.setCharacterSize(10);
	m_picture.setTexture(rmgr.getTexture(ResourceSection::Base, "default.png"));
}

Button::~Button()
{
	ResourceManager::getInstance().unloadSection(ResourceSection::Menu);
}


Button::Button(const std::string &pathImage , const std::string &text , const sf::Vector2f &position) :
	m_text() , m_position(position)
{
	ResourceManager &rmgr = ResourceManager::getInstance();
	//Load all textures & fonts
	rmgr.loadSection(ResourceSection::Menu);
	m_text.setString(text);
	m_text.setFont(rmgr.getFont(ResourceSection::Base , "ASafePlacetoFall.ttf"));
	m_text.setPosition(m_position);
	m_text.setCharacterSize(10);
	m_picture.setTexture(rmgr.getTexture(ResourceSection::Base, pathImage));
}

void Button::draw(sf::RenderTarget &target , sf::RenderStates) const
{
	target.draw(m_picture);
	target.draw(m_text);
}

void Button::setPosition(const sf::Vector2f &position)
{
	m_position = position;
}

void Button::setPicture(const std::string &path)
{
	ResourceManager &rmgr = ResourceManager::getInstance();
	//Load all textures & fonts
	rmgr.loadSection(ResourceSection::Menu);
	m_picture.setTexture(rmgr.getTexture(ResourceSection::Base, path));
}

void Button::setText(const std::string &text)
{
	m_text.setString(text);
}


void Button::onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt){}
void Button::onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt){}
void Button::onMouseMoved(const sf::Event::MouseMoveEvent &evt){}
bool Button::isInButton(const sf::Vector2f &positionCursor){}
