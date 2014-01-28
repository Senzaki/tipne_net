#include "Menu.hpp"
#include "Application.hpp"
#include "ResourceManager.hpp"
#include "Translator.hpp"
#include "Button.hpp"
#include "Checkbox.hpp"
#include "Config.hpp"
#include "DecoratedLineEdit.hpp"
#include <iostream>

Menu::Menu(sf::RenderWindow &window, float vratio, float xyratio):
	m_window(window),
	m_guimgr(window),
	m_camera(sf::FloatRect(0.f, 0.f, xyratio * DEFAULT_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT)),
	m_vratio(vratio),
	m_xyratio(xyratio)
{
	Translator::getInstance().loadPackage("menu");
	//Load all textures & fonts
	ResourceManager &rsmgr = ResourceManager::getInstance();
	m_cursor.setTexture(rsmgr.getTexture(ResourceSection::Base, Resource::CURSOR_TEX));
}

Menu::~Menu()
{
}

void Menu::load()
{
	//Create buttons
	m_guimgr.clear();
	Widget *topwidget = m_guimgr.getTopWidget();

	constexpr const int BUTTONS_COUNT = 4;
	Widget *buttons[BUTTONS_COUNT];//Array only used for loop
	buttons[0] = new Button(topwidget, tr("play"));
	buttons[1] = new Button(topwidget, tr("options"), std::bind(&Menu::loadOptions, this));
	buttons[2] = new Button(topwidget, tr("credits"));
	buttons[3] = new Button(topwidget, tr("quit"), std::bind(&Application::setNextAppState, &Application::getInstance(), nullptr, true));
	//Set the appropriate position
	float interval = (m_window.getSize().y - 100.f) / BUTTONS_COUNT;
	for(unsigned int i = 0; i < BUTTONS_COUNT; i++)
		buttons[i]->setPosition((m_window.getSize().x - buttons[i]->getSize().x) / 2.f, 100.f + i * interval);
}

void Menu::update(float etime)
{
	//Update GUI
	m_guimgr.update(etime);
}

void Menu::draw()
{
	m_window.clear();
	//Draw GUI
	m_guimgr.draw();
	//Apply the scaling view
	m_window.setView(m_camera);
	//Draw every scaled thing
	m_window.draw(m_cursor);
	//Back to default view
	m_window.setView(m_window.getDefaultView());
}

void Menu::onWindowClosed()
{
	Application::getInstance().setNextAppState(nullptr);
}

void Menu::onKeyPressed(const sf::Event::KeyEvent &evt)
{
	switch(evt.code)
	{
		case sf::Keyboard::Escape:
			Application::getInstance().setNextAppState(nullptr);
			break;

		default:
			break;
	}
	m_guimgr.onKeyPressed(evt);
}

void Menu::onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt)
{
	m_guimgr.onMouseButtonPressed(evt);
	sf::Vector2f camcoords(evt.x / m_vratio, evt.y / m_vratio);
}

void Menu::onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt)
{
	m_guimgr.onMouseButtonReleased(evt);
	sf::Vector2f camcoords(evt.x / m_vratio, evt.y / m_vratio);
}

void Menu::onTextEntered(const sf::Event::TextEvent &evt)
{
	m_guimgr.onTextEntered(evt);
}

void Menu::onMouseMoved(const sf::Event::MouseMoveEvent &evt)
{
	m_guimgr.onMouseMoved(evt);
	sf::Vector2f camcoords(evt.x / m_vratio, evt.y / m_vratio);
	//Cursor is displayed in camera coords, but mouse coordinates are given in window coordinates
	m_cursor.setPosition(camcoords);
}

void Menu::loadOptions()
{
	m_guimgr.clear();
	//Widgets to add
	Widget *topwidget = m_guimgr.getTopWidget();
	DecoratedLineEdit *lineedit;
	constexpr const int CHECKBOX_COUNT = 3;
	CheckBox *checkboxes[CHECKBOX_COUNT];
	Button *button[2];

	lineedit = new DecoratedLineEdit(topwidget, 150, 0, [](std::string name){Config::getInstance().name = name;});

	button[0] = new Button(topwidget, tr("save"), std::bind(&Config::save, &Config::getInstance()));
	button[1] = new Button(topwidget, tr("menu"), std::bind(&Menu::load, this));

	checkboxes[0] = new CheckBox(topwidget, [](){Config::getInstance().fullscreen = 1;}, [](){Config::getInstance().fullscreen = 0;});
	checkboxes[1] = new CheckBox(topwidget, [](){Config::getInstance().vsync = 1;}, [](){Config::getInstance().vsync = 0;});
	checkboxes[2] = new CheckBox(topwidget, [](){Config::getInstance().dispfreq = 1;}, [](){Config::getInstance().dispfreq = 0;});

	lineedit->setString(Config::getInstance().name);
	lineedit->setPosition(50, 200);

	button[0]->setPosition(450, 500);
	button[1]->setPosition(380, 500);

	checkboxes[0]->setChecked(Config::getInstance().fullscreen);
	checkboxes[1]->setChecked(Config::getInstance().vsync);
	checkboxes[2]->setChecked(Config::getInstance().dispfreq);
	float interval = (m_window.getSize().y - 100.f) / CHECKBOX_COUNT;
	for(int i = 0 ; i < CHECKBOX_COUNT ; i++)
		checkboxes[i]->setPosition((m_window.getSize().x - checkboxes[i]->getSize().x) / 2.f, 100.f + i * interval);
}
