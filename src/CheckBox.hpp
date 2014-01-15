#ifndef CHECKBOX_HPP_INCLUDED
#define CHECKBOX_HPP_INCLUDED

#include "Widget.hpp"

class CheckBox : public Widget
{
	public:
	CheckBox(Widget *parent = nullptr , std::function<void()> callbackcheck = std::function<void()>() , std::function<void()> callbackuncheck = std::function<void()>());

	void setCallbackCheck(std::function<void()> callback);
	void setCallbackUncheck(std::function<void()> callback);

	const bool &isChecked();

	virtual void draw(sf::RenderWindow &window);

	protected:
	virtual void onPositionChanged();

	virtual bool onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt);

	private:
	bool m_isChecked;
	sf::Sprite m_background;
	std::function<void()> m_funcCheck;
	std::function<void()> m_funcUncheck;
};

#endif //CHECKBOX_HPP_INCLUDED
