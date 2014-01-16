#ifndef CHECKBOX_HPP_INCLUDED
#define CHECKBOX_HPP_INCLUDED

#include "Widget.hpp"

class CheckBox : public Widget
{
	public:
	CheckBox(Widget *parent = nullptr, std::function<void()> callbackcheck = std::function<void()>(), std::function<void()> callbackuncheck = std::function<void()>());

	void setCheckCallback(std::function<void()> callback);
	void setUncheckCallback(std::function<void()> callback);

	void setChecked(bool checked);
	bool isChecked() const;

	virtual void draw(sf::RenderWindow &window);

	protected:
	virtual void onPositionChanged();
	virtual bool onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt);
	virtual bool onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt);

	private:
	bool m_checked;
	sf::Sprite m_background;
	std::function<void()> m_funccheck;
	std::function<void()> m_funcuncheck;
};

#endif //CHECKBOX_HPP_INCLUDED
