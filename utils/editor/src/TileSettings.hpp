#ifndef TILESETTINGS_HPP_INCLUDED
#define TILESETTINGS_HPP_INCLUDED

#include "Button.hpp"
#include "Widget.hpp"
#include "Checkbox.hpp"
#include "GuiManager.hpp"
#include "Label.hpp"
#include "LineEdit.hpp"

class TileSettings : public Widget
{
	public:
	TileSettings(Widget *parent = nullptr);
	virtual ~TileSettings();

	private:
	Label *m_label;
	LineEdit *m_appearence;
	CheckBox *m_passable;
	void setWidgetsPositions();
};

#endif // TILESETTINGS_HPP_INCLUDED
