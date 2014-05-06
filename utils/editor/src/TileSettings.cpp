#include "TileSettings.hpp"
#include "Label.hpp"
#include "DecoratedLineEdit.hpp"
#include "CheckBox.hpp"
#include <sstream>
#include <iostream>

TileSettings::TileSettings(Widget *parent) : Widget(parent)
{
	setSize(1024.f, 768.f);
	m_label = new Label(this, "Appearence :");
	m_label2 = new Label(this, "Passable :");
	m_appearence = new DecoratedLineEdit(this, 175.f);
	m_passable = new CheckBox(this);
	setWidgetsPositions();
}

TileSettings::~TileSettings()
{

}

void TileSettings::setWidgetsPositions()
{
	m_label->setPosition(115.f, 370.f);
	m_label2->setPosition(120.f, 250.f);
	m_appearence->setPosition(62.f, 400.f);
	m_passable->setPosition(145.f, 290.f);
}

void TileSettings::setTile(Tile tile)
{
	m_passable->setChecked(tile.passable);

	std::string string(" ");
	std::ostringstream oss;
	oss << std::hex << tile.appearance;
	string = oss.str();
	m_appearence->setString(string);
}


Tile TileSettings::getUpdatedTile()
{
	Tile tile;
	std::istringstream iss;
	sf::Uint16 code;
	iss.str(this->m_appearence->getString());
	iss >> std::hex >> code;

	tile.appearance = code;

	tile.passable = this->m_passable->isChecked();
	return tile;
}

