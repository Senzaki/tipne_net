#include "OptionsMenu.hpp"
#include "Translator.hpp"
#include "Config.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>

OptionsMenu::OptionsMenu(Widget *parent, std::function<void()> callback):
	Widget(parent),
	m_callback(callback),
	m_name(new DecoratedLineEdit(this, 150)),
	m_vsync(new CheckBox(this)),
	m_fullscreen(new CheckBox(this)),
	m_videomode(new Label(this)),
	m_lang(new Label(this)),
	m_cancel(new Button(this, tr("cancel"), std::bind(&OptionsMenu::cancel, this))),
	m_save(new Button(this, tr("save"), std::bind(&OptionsMenu::saveOptions, this))),
	m_curvmode(0),
	m_curlang(0)
{
	setSize(getParent()->getSize());
	getAvailablesVideoModes();

	//TEMP
	m_langs.push_back("en");
	m_langs.push_back("fr");


	//Labels
	m_labels[OPTIONS_NAME] = new Label(this, tr("name"));
	m_labels[OPTIONS_VIDEOMODE] = new Label(this, tr("videomode"));
	m_labels[OPTIONS_VSYNC] = new Label(this, tr("vsync"));
	m_labels[OPTIONS_FULLSCREEN] = new Label(this, tr("fullscreen"));
	m_labels[OPTIONS_LANG] = new Label(this, tr("lang"));

	std::string left = " < ";
	std::string right = " > ";
	m_videomodebuttons[0] = new Button(this, left, std::bind(&OptionsMenu::changeVideoMode, this, -1));
	m_videomodebuttons[1] = new Button(this, right, std::bind(&OptionsMenu::changeVideoMode, this, 1));
	m_langbuttons[0] = new Button(this, left, std::bind(&OptionsMenu::changeLanguage, this, -1));
	m_langbuttons[1] = new Button(this, right, std::bind(&OptionsMenu::changeLanguage, this, 1));

	setDefaultValues();
	setWidgetsPosition();
}

OptionsMenu::~OptionsMenu()
{

}

void OptionsMenu::setWidgetsPosition()
{
	const float interval = (getSize().y - 200.f) / OPTIONS_LABEL_COUNT;
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
	//Values of lineedits set as values of config
	m_name->setString(Config::getInstance().name);
	m_fullscreen->setChecked(Config::getInstance().fullscreen);
	m_vsync->setChecked(Config::getInstance().vsync);
	std::ostringstream oss;
	oss << Config::getInstance().width << "x" << Config::getInstance().height;
	m_videomode->setString(oss.str());
	m_lang->setString(tr("language_name"));

	//Define m_curlang
	while(Config::getInstance().lang != m_langs[m_curlang] && m_curlang < (int)m_langs.size() - 1)
	{
		++m_curlang;
	}

	//Define m_curvmode
	while((Config::getInstance().width != m_vmodes[m_curvmode].width || Config::getInstance().height != m_vmodes[m_curvmode].height) && m_curvmode < (int)m_vmodes.size())
	{
		++m_curvmode;
	}
	if(m_curvmode == (int)m_vmodes.size())
		m_curvmode = 0;
}

void OptionsMenu::saveOptions()
{
	Config::getInstance().name = m_name->getString();
	Config::getInstance().fullscreen = m_fullscreen->isChecked();
	Config::getInstance().vsync = m_vsync->isChecked();
	Config::getInstance().height = m_vmodes[m_curvmode].height;
	Config::getInstance().width = m_vmodes[m_curvmode].width;
	Config::getInstance().lang = m_langs[m_curlang];
	Config::getInstance().save();
	if(m_callback)
	{
		m_callback();
	}
}

void OptionsMenu::cancel()
{
	//Reload the adequate packages of translation
	Translator::getInstance().loadPackage("base");
	Translator::getInstance().loadPackage("menu");
	if(m_callback)
	{
		m_callback();
	}
}

void OptionsMenu::changeVideoMode(int direction)
{
	if(!((m_curvmode == 0 && direction == -1) || (m_curvmode == (int)m_vmodes.size() - 1 && direction == 1)))
		m_curvmode += direction;
	std::ostringstream oss;
	oss.str("");
	oss << m_vmodes[m_curvmode].width << "x" << m_vmodes[m_curvmode].height;
	m_videomode->setString(oss.str());

}

void OptionsMenu::changeLanguage(int direction)
{
	if(m_curlang == 0 && direction == -1)
		m_curlang = m_langs.size() - 1;
	else if(m_curlang == (int)m_langs.size() - 1 && direction == 1)
		m_curlang = 0;
	else
		m_curlang += direction;
	//Temp string to keep the lang if the save button is not pressed
	std::string temp(Config::getInstance().lang);
	Config::getInstance().lang = m_langs[m_curlang];

	//Load the new packages
	Translator::getInstance().loadPackage("base");
	Translator::getInstance().loadPackage("menu");

	//update strings
	m_lang->setString(tr("language_name"));
	m_labels[OPTIONS_NAME]->setString(tr("name"));
	m_labels[OPTIONS_VIDEOMODE]->setString(tr("videomode"));
	m_labels[OPTIONS_VSYNC]->setString(tr("vsync"));
	m_labels[OPTIONS_FULLSCREEN]->setString(tr("fullscreen"));
	m_labels[OPTIONS_LANG]->setString(tr("lang"));
	m_save->setText(tr("save"));
	m_cancel->setText(tr("cancel"));

	//move the buttons to avoid a display bug
	const float interval = (getSize().y - 200.f) / OPTIONS_LABEL_COUNT;
	m_save->setPosition(getSize().x / 3.f, getSize().y - interval);
	m_cancel->setPosition(2 * getSize().x / 3.f, getSize().y - interval);

	//Reset the language
	Config::getInstance().lang = temp;
}

void OptionsMenu::getAvailablesVideoModes()
{
	std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
	//Get availables video modes
	for(unsigned int i = 0 ; i < modes.size() ; ++i)
	{
		if(modes[i].width >= 800 && modes[i].height >= 600 && modes[i].bitsPerPixel == 32)
			m_vmodes.push_back(modes[i]);
	}
	//Sort videomodes
	std::sort(m_vmodes.begin(), m_vmodes.end());
}
