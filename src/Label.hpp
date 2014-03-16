#ifndef LABEL_HPP_INCLUDED
#define LABEL_HPP_INCLUDED

#include "Widget.hpp"

class Label : public Widget
{
	public:
	Label(Widget *parent = nullptr, std::string string = "");

	void setString(std::string);
	virtual void draw(sf::RenderWindow &window);

	protected:
	virtual void onPositionChanged();

	private:
	void updateSize();
	sf::Text m_text;
};

#endif
