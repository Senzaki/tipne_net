#ifndef TILESETTINGS_HPP_INCLUDED
#define TILESETTINGS_HPP_INCLUDED

#include "Button.hpp"
#include "Widget.hpp"
#include "CheckBox.hpp"
#include "GUIManager.hpp"
#include "Label.hpp"
#include "DecoratedLineEdit.hpp"
#include "Tile.hpp"

class TileSettings : public Widget
{
	public:
	TileSettings(Widget *parent = nullptr);
	virtual ~TileSettings();
	void setTile(Tile tile);
	Tile getUpdatedTile();

	private:
	Label *m_label, *m_label2;
	DecoratedLineEdit *m_appearence;
	CheckBox *m_passable;
	void setWidgetsPositions();
};

#endif // TILESETTINGS_HPP_INCLUDED
