#include "Menu.hpp"
#include "Application.hpp"
#include "ResourceManager.hpp"
#include "Translator.hpp"
#include "Button.hpp"
#include "CheckBox.hpp"
#include "DecoratedLineEdit.hpp"
#include "Label.hpp"
#include "Config.hpp"
#include "GameAppState.hpp"
#include "ClientSimulator.hpp"
#include "ServerSimulator.hpp"
#include "NetworkCodes.hpp"
#include <sstream>
#include <iostream>

Menu::Menu(sf::RenderWindow &window, float vratio, float xyratio):
	m_window(window),
	m_guimgr(window),
	m_camera(sf::FloatRect(0.f, 0.f, xyratio * DEFAULT_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT)),
	m_vratio(vratio),
	m_xyratio(xyratio)
{

}

Menu::~Menu()
{

}

void Menu::load()
{
	Translator::getInstance().loadPackage("menu");

	//Load all textures & fonts
	ResourceManager &rsmgr = ResourceManager::getInstance();
	m_cursor.setTexture(rsmgr.getTexture(ResourceSection::Base, Resource::CURSOR_TEX));

	showMainMenu();
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
	if(!m_guimgr.onKeyPressed(evt))
	{
		switch(evt.code)
		{
			case sf::Keyboard::Escape:
				Application::getInstance().setNextAppState(nullptr);
				break;

			default:
				break;
		}
	}
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

void Menu::onMouseMoved(const sf::Event::MouseMoveEvent &evt)
{
	m_guimgr.onMouseMoved(evt);
	sf::Vector2f camcoords(evt.x / m_vratio, evt.y / m_vratio);
	//Cursor is displayed in camera coords, but mouse coordinates are given in window coordinates
	m_cursor.setPosition(camcoords);
}

void Menu::onTextEntered(const sf::Event::TextEvent &evt)
{
	m_guimgr.onTextEntered(evt);
}

void Menu::showMainMenu()
{
	m_guimgr.clear();

	//Create buttons
	Widget *topwidget = m_guimgr.getTopWidget();
	const unsigned int BUTTONS_COUNT = 4;
	Widget *buttons[BUTTONS_COUNT];//Array only used for loop
	buttons[0] = new Button(topwidget, tr("play"), std::bind(&Menu::TEMPtestPlay, this));
	buttons[1] = new Button(topwidget, tr("options"), std::bind(&Menu::showOptions, this));
	buttons[2] = new Button(topwidget, tr("credits"));
	buttons[3] = new Button(topwidget, tr("quit"), std::bind(&Application::setNextAppState, &Application::getInstance(), nullptr, true));
	//Set the appropriate position
	float interval = (m_window.getSize().y - 100.f) / BUTTONS_COUNT;
	for(unsigned int i = 0; i < BUTTONS_COUNT; i++)
		buttons[i]->setPosition((m_window.getSize().x - buttons[i]->getSize().x) / 2.f, 100.f + i * interval);
}

void Menu::showOptions()
{
	m_guimgr.clear();

	//Widgets to add
	Widget *topwidget = m_guimgr.getTopWidget();

	Button *cancel = new Button(topwidget, tr("cancel"), std::bind(&Menu::showMainMenu, this));
	cancel->setPosition(450, 500);
	Button *save = new Button(topwidget, tr("save"), std::bind(&Menu::saveOptions, this));
	save->setPosition(300, 500);
	//new DecoratedLineEdit(topwidget, 150, 0);

	//Create the labels
	constexpr const int LABEL_COUNT = 6;
	Label *labels[LABEL_COUNT];
	labels[0] = new Label(topwidget, tr("name"));
	labels[1] = new Label(topwidget, tr("videomode"));
	labels[2] = new Label(topwidget, tr("fullscreen"));
	labels[3] = new Label(topwidget, tr("vsync"));
	labels[4] = new Label(topwidget, tr("dispfreq"));
	labels[5] = new Label(topwidget, tr("lang"));
	//Set their position
	float interval = (m_window.getSize().y - 200.f) / LABEL_COUNT;
	for (int i = 0 ; i < LABEL_COUNT ; i++)
		labels[i]->setPosition((m_window.getSize().x - labels[i]->getSize().x) / 2.f - m_window.getSize().x / 4.f, 100.f + i * interval);

	//Create other widgets
	DecoratedLineEdit *name = new DecoratedLineEdit(topwidget, 150);
	Label *videomode = new Label(topwidget);
	CheckBox *fullscreen = new CheckBox(topwidget);
	CheckBox *vsync = new CheckBox(topwidget);
	Label *dispfreq = new Label(topwidget);
	Label *lang = new Label(topwidget);

	Button *videomodebuttons[2];
	std::string left = " < ";
	std::string right = " > ";
	videomodebuttons[0] = new Button(topwidget, left);
	videomodebuttons[1] = new Button(topwidget, right);
	Button *dispfreqbuttons[2];
	dispfreqbuttons[0] = new Button(topwidget, left);
	dispfreqbuttons[1] = new Button(topwidget, right);
	Button *langbuttons[2];
	langbuttons[0] = new Button(topwidget, left);
	langbuttons[1] = new Button(topwidget, right);

	//Set their default values
	name->setString(Config::getInstance().name);
	fullscreen->setChecked(Config::getInstance().fullscreen);
	vsync->setChecked(Config::getInstance().vsync);

	std::ostringstream oss;
	oss << Config::getInstance().width << "x" << Config::getInstance().height;
	videomode->setString(oss.str());
	oss.str("");
	oss << Config::getInstance().dispfreq;
	dispfreq->setString(oss.str());
	lang->setString(tr(Config::getInstance().lang));

	//Set their positions
	name->setPosition((m_window.getSize().x - name->getSize().x) / 2.f, labels[0]->getAbsolutePosition().y);
	videomode->setPosition((m_window.getSize().x - videomode->getSize().x) / 2.f, labels[1]->getAbsolutePosition().y);
	fullscreen->setPosition((m_window.getSize().x - fullscreen->getSize().x) / 2.f, labels[2]->getAbsolutePosition().y);
	vsync->setPosition((m_window.getSize().x - vsync->getSize().x) / 2.f, labels[3]->getAbsolutePosition().y);
	dispfreq->setPosition((m_window.getSize().x - dispfreq->getSize().x) / 2.f, labels[4]->getAbsolutePosition().y);
	lang->setPosition((m_window.getSize().x - lang->getSize().x) / 2.f, labels[5]->getAbsolutePosition().y);

	videomodebuttons[0]->setPosition(videomode->getAbsolutePosition().x - videomodebuttons[0]->getSize().x - 10.f, videomode->getAbsolutePosition().y - videomodebuttons[0]->getSize().y / 2.f);
	dispfreqbuttons[0]->setPosition(dispfreq->getAbsolutePosition().x - dispfreqbuttons[0]->getSize().x - 10.f, dispfreq->getAbsolutePosition().y - dispfreqbuttons[0]->getSize().y / 2.f);
	langbuttons[0]->setPosition(lang->getAbsolutePosition().x - langbuttons[0]->getSize().x - 10.f, lang->getAbsolutePosition().y - langbuttons[0]->getSize().y / 2.f);
	videomodebuttons[1]->setPosition(videomode->getAbsolutePosition().x + videomode->getSize().x + 10.f, videomode->getAbsolutePosition().y - videomodebuttons[1]->getSize().y / 2.f);
	dispfreqbuttons[1]->setPosition(dispfreq->getAbsolutePosition().x + dispfreq->getSize().x + 10.f, dispfreq->getAbsolutePosition().y - dispfreqbuttons[1]->getSize().y / 2.f);
	langbuttons[1]->setPosition(lang->getAbsolutePosition().x + lang->getSize().x + 10.f, lang->getAbsolutePosition().y - langbuttons[1]->getSize().y / 2.f);
}

void Menu::saveOptions()
{
	showMainMenu();
}

void Menu::TEMPtestPlay()
{
	Config &config = Config::getInstance();
	GameSimulator *simulator = new ClientSimulator();
	int status;
	std::cout << "Trying as client..." << std::endl;
	if((status = static_cast<ClientSimulator *>(simulator)->startNetThread(sf::IpAddress(config.connectto_ip), config.connectto_port, config.name)) != (int)ConnectionStatus::Accepted)
	{
		delete simulator;
		if(status == (int)ConnectionStatus::GameIsFull)
		{
			std::cout << "[Game is full]" << std::endl;
			simulator = nullptr;
		}
		else
		{
			std::cout << "[Client failed]" << std::endl;
			std::cout << "Trying as server... " << std::endl;
			simulator = new ServerSimulator(false);
			if(!static_cast<ServerSimulator *>(simulator)->loadMap((sf::Uint8)MapId::Default))
			{
				std::cout << "[Server failed]" << std::endl;
				delete simulator;
				simulator = nullptr;
			}
			else if(!static_cast<ServerSimulator *>(simulator)->startNetThread(config.server_port, config.max_players))
			{
				std::cout << "[Server failed]" << std::endl;
				delete simulator;
				simulator = nullptr;
			}
			else
				std::cout << "[Server ok]" << std::endl;
		}
	}
	else
		std::cout << "[Client ok]" << std::endl;
	if(simulator)
	{
		GameAppState *next = new GameAppState(m_window, m_vratio, m_xyratio, simulator);
		Application::getInstance().setNextAppState(next);
	}
	else
		Application::getInstance().setNextAppState(nullptr);
}
