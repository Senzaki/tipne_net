#ifndef BUTTON_HPP_INCLUDED
#define BUTTON_HPP_INCLUDED

#include <SFML/Graphics.hpp>

class Button : public sf::Drawable
{
	public:
	Button();
	Button(const std::string &pathImage , const std::string &text , const sf::Vector2f &position = sf::Vector2f(0 , 0));
	virtual ~Button();
	virtual void onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt);
	virtual void onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt);
	virtual void onMouseMoved(const sf::Event::MouseMoveEvent &evt);
	bool isInButton(const sf::Vector2f &positionCursor);
	void setPosition(const sf::Vector2f &position);
	void setPicture(const std::string &path);
	void setText(const std::string &text);

	protected:
	virtual void draw(sf::RenderTarget &target , sf::RenderStates) const;

	private:
	sf::Sprite m_picture;
	sf::Text m_text;
	sf::Vector2f m_position;
};

#endif // BUTTON_HPP_INCLUDED
