#ifndef LINEEDIT_HPP_INCLUDED
#define LINEEDIT_HPP_INCLUDED

#include <functional>
#include "Widget.hpp"

class LineEdit : public Widget
{
	public:
	LineEdit(Widget *parent = nullptr, float width = 0, std::function<void(std::string)> callback = std::function<void(std::string)>());
	virtual ~LineEdit();

	void setMaxChar(unsigned int maxchar);
	void setWidth(float width);
	void setCallback(std::function<void(std::string)> callback);
	void setString(const std::string &string);
	std::string getString() const;

	virtual void draw(sf::RenderWindow &window);

	protected:
	virtual void onPositionChanged();
	virtual bool onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt);
	virtual bool onTextEntered(const sf::Event::TextEvent &evt);
	virtual bool onKeyPressed(const sf::Event::KeyEvent &evt);

	private:
	void updateCursor();
	bool m_inputison;
	unsigned int m_maxchar; //0 if not needed
	unsigned int m_positioncursor;
	float m_width;
	sf::Text m_text;
	sf::RectangleShape m_cursor;
	std::function<void(std::string)> m_func;
};

#endif //LINEEDIT_HPP_INCLUDED
