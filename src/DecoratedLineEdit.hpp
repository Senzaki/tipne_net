#ifndef DECORATEDLINEEDIT_HPP_INCLUDED
#define DECORATEDLINEEDIT_HPP_INCLUDED

#include "Widget.hpp"
#include "LineEdit.hpp"


class DecoratedLineEdit : public Widget
{
	public:
	DecoratedLineEdit(Widget *parent = nullptr, float width = 0, std::function<void(std::string)> callback = std::function<void(std::string)>());
	virtual ~DecoratedLineEdit();

	void setMaxChar(unsigned int maxchar);
	void setWidth(float width);
	void setCallback(std::function<void(std::string)> callback);
	void setString(std::string string);
	std::string getString() const;

	virtual void draw(sf::RenderWindow &window);

	protected:
	virtual void onPositionChanged();

	private:
	void updateSize();

	LineEdit *m_lineedit;
	float m_width;
	sf::VertexArray m_background;
	sf::RenderStates m_rdstates;

};

#endif //DECORATEDLINEEDIT_HPP_INCLUDED
