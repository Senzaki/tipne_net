#ifndef BUTTON_HPP_INCLUDED
#define BUTTON_HPP_INCLUDED

#include "Widget.hpp"

class Button: public Widget
{
	public:
	Button(Widget *parent = nullptr, std::function<void()> callback = std::function<void()>());
	Button(Widget *parent, const std::string &text, std::function<void()> callback = std::function<void()>());

	void setText(const std::string &text);
	void setCallback(std::function<void()> callback);

	void setMinimumWidth(float minwidth);

	void draw(sf::RenderWindow &window);

	protected:
	virtual void onPositionChanged();

	virtual bool onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt);
	virtual bool onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt);
	virtual void onMouseEntered(const sf::Event::MouseMoveEvent &evt);
	virtual void onMouseLeft();

	private:
	void updateSize();

	sf::VertexArray m_background;
	sf::RenderStates m_rdstates;
	sf::Text m_text;
	float m_minw;

	bool m_clicked;
	std::function<void()> m_func;
};

#endif // BUTTON_HPP_INCLUDED
