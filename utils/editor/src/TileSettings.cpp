#include "TileSettings.hpp"
#include "Label.hpp"
#include "LineEdit.hpp"
#include "Checkbox.hpp"

TileSettings::TileSettings(Widget *parent) : Widget(parent)
{
	setSize(1024.f, 768.f);
	m_label = new Label(this, "ta mere");
	m_appearence = new LineEdit(this, 200.f);
	m_passable = new CheckBox(this);
	setWidgetsPositions();
}

TileSettings::~TileSettings()
{

}

void TileSettings::setWidgetsPositions()
{
	m_label->setPosition(400.f, 400.f);
	m_appearence->setPosition(400.f, 600.f);
	m_passable->setPosition(500.f, 800.f);
}
