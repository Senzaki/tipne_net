#ifndef BUTTON_HPP_INCLUDED
#define BUTTON_HPP_INCLUDED

#include <SFML/Graphics.hpp>

class Button
{
	public:
	Button(std::function<void()> callback = std::function<void()>());
	Button(const std::string &text, std::function<void()> callback = std::function<void()>());
	Button(const std::string &text, const sf::Font &font, const sf::Texture &normaltex, const sf::Texture &hltex, const sf::Texture &clicktex, std::function<void()> callback = std::function<void()>());

	void resetResources();

	void setNormalTexture(const sf::Texture &tex);
	void setNormalCharColor(const sf::Color &color);
	void setHighlightTexture(const sf::Texture &tex);
	void setHighlightCharColor(const sf::Color &color);
	void setClickedTexture(const sf::Texture &tex);
	void setClickedCharColor(const sf::Color &color);
	void setFont(const sf::Font &font);
	void setText(const std::string &text);
	void setCallback(std::function<void()> callback);

	void setPosition(float x, float y);
	const sf::FloatRect &getGlobalBounds() const;

	void onMouseButtonPressed(const sf::Vector2f &coords);
	void onMouseButtonReleased(const sf::Vector2f &coords);
	void onMouseMoved(const sf::Vector2f &coords);

	void draw(sf::RenderWindow &window);

	private:
	void updateBackground();

	const sf::Texture *m_stdtex;
	sf::Color m_stdcol;
	const sf::Texture *m_hltex;
	sf::Color m_hlcol;
	const sf::Texture *m_cltex;
	sf::Color m_clcol;
	sf::Sprite m_background;
	sf::Text m_text;
	sf::FloatRect m_rect;

	bool m_highlighted;
	bool m_clicked;
	std::function<void()> m_func;
};

#endif // BUTTON_HPP_INCLUDED
