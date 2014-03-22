#include "OptionsMenu.hpp"
#include "Translator.hpp"
#include "Config.hpp"
#include <sstream>

OptionsMenu::OptionsMenu(Widget *parent, std::function<void()> callbacksave, std::function<void()> callbackcancel):
	Widget(parent)
{
	setSize(getParent()->getSize());
	m_callbacksave = callbacksave;

	m_save = new Button(this, tr("save"), std::bind(&OptionsMenu::saveOptions, this));
	m_cancel = new Button(this, tr("cancel"), callbackcancel);
	m_vsync = new CheckBox(this);
	m_fullscreen = new CheckBox(this);
	m_videomode = new Label(this);
	m_lang = new Label(this);
	m_name = new DecoratedLineEdit(this, 150);

	//Labels
	m_labels[OPTIONS_NAME] = new Label(this, tr("name"));
	m_labels[OPTIONS_FULLSCREEN] = new Label(this, tr("fullscreen"));
	m_labels[OPTIONS_LANG] = new Label(this, tr("lang"));
	m_labels[OPTIONS_VSYNC] = new Label(this, tr("vsync"));
	m_labels[OPTIONS_VIDEOMODE] = new Label(this, tr("videomode"));

	std::string left = " < ";
	std::string right = " > ";
	m_videomodebuttons[0] = new Button(this, left);
	m_videomodebuttons[1] = new Button(this, right);
	m_langbuttons[0] = new Button(this, left);
	m_langbuttons[1] = new Button(this, right);

	setDefaultValues();
	setWidgetsPosition();
}

OptionsMenu::~OptionsMenu()
{

}

bool OptionsMenu::onKeyPressed(const sf::Event::KeyEvent &evt)
{
	return false;
}

void OptionsMenu::setWidgetsPosition()
{
	float interval = (getSize().y - 200.f) / OPTIONS_LABEL_COUNT;
	for (int i = 0 ; i < OPTIONS_LABEL_COUNT ; i++)
		m_labels[i]->setPosition((getSize().x - m_labels[i]->getSize().x) / 2.f - getSize().x / 4.f, 100.f + i * interval);

	//Postitions of options
	m_name->setPosition((getSize().x - m_name->getSize().x) / 2.f, m_labels[OPTIONS_NAME]->getAbsolutePosition().y);
	m_videomode->setPosition((getSize().x - m_videomode->getSize().x) / 2.f, m_labels[OPTIONS_VIDEOMODE]->getPosition().y);
	m_fullscreen->setPosition((getSize().x - m_fullscreen->getSize().x) / 2.f, m_labels[OPTIONS_FULLSCREEN]->getPosition().y);
	m_vsync->setPosition((getSize().x - m_vsync->getSize().x) / 2.f, m_labels[OPTIONS_VSYNC]->getPosition().y);
	m_lang->setPosition((getSize().x - m_lang->getSize().x) / 2.f, m_labels[OPTIONS_LANG]->getPosition().y);

	//Positions of buttons
	m_videomodebuttons[0]->setPosition(m_videomode->getPosition().x - m_videomodebuttons[0]->getSize().x - 10.f, m_videomode->getPosition().y - m_videomodebuttons[0]->getSize().y / 2.f);
	m_langbuttons[0]->setPosition(m_lang->getPosition().x - m_langbuttons[0]->getSize().x - 10.f, m_lang->getPosition().y - m_langbuttons[0]->getSize().y / 2.f);
	m_videomodebuttons[1]->setPosition(m_videomode->getPosition().x + m_videomode->getSize().x + 10.f, m_videomode->getPosition().y - m_videomodebuttons[1]->getSize().y / 2.f);
	m_langbuttons[1]->setPosition(m_lang->getPosition().x + m_lang->getSize().x + 10.f, m_lang->getPosition().y - m_langbuttons[1]->getSize().y / 2.f);
	m_save->setPosition(getSize().x / 3.f, getSize().y - interval);
	m_cancel->setPosition(2 * getSize().x / 3.f, getSize().y - interval);
}

void OptionsMenu::setDefaultValues()
{
	m_name->setString(Config::getInstance().name);
	m_fullscreen->setChecked(Config::getInstance().fullscreen);
	m_vsync->setChecked(Config::getInstance().vsync);
	std::ostringstream oss;
	oss << Config::getInstance().width << "x" << Config::getInstance().height;
	m_videomode->setString(oss.str());
	m_lang->setString(tr(Config::getInstance().lang));
}

void OptionsMenu::saveOptions()
{
	Config::getInstance().name = m_name->getString();
	Config::getInstance().fullscreen = m_fullscreen->isChecked();
	Config::getInstance().vsync = m_vsync->isChecked();
	//need to add the language and the video mode
	Config::getInstance().save();
	if(m_callbacksave)
	{
		m_callbacksave();
	}
}

