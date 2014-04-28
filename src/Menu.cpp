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
#include "OptionsMenu.hpp"
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
	const unsigned int BUTTONS_COUNT = 5;
	Widget *buttons[BUTTONS_COUNT];//Array only used for loop
	buttons[0] = new Button(topwidget, tr("host"), std::bind(&Menu::showHostMenu, this));
	buttons[1] = new Button(topwidget, tr("connect"), std::bind(&Menu::showConnectMenu, this));
	buttons[2] = new Button(topwidget, tr("options"), std::bind(&Menu::showOptions, this));
	buttons[3] = new Button(topwidget, tr("credits"));
	buttons[4] = new Button(topwidget, tr("quit"), std::bind(&Application::setNextAppState, &Application::getInstance(), nullptr, true));
	//Set the appropriate position
	float interval = (m_window.getSize().y - 100.f) / BUTTONS_COUNT;
	for(unsigned int i = 0; i < BUTTONS_COUNT; i++)
		buttons[i]->setPosition((m_window.getSize().x - buttons[i]->getSize().x) / 2.f, 100.f + i * interval);
}

void Menu::showOptions()
{
	m_guimgr.clear();
	Widget *topwidget = m_guimgr.getTopWidget();
	new OptionsMenu(topwidget, std::bind(&Menu::showMainMenu, this));
}

void Menu::showConnectMenu()
{
	m_guimgr.clear();
	Widget *topwidget = m_guimgr.getTopWidget();

	Label *portlabel = new Label(topwidget, tr("port"));
	Label *iplabel = new Label(topwidget, tr("ipadress"));
	DecoratedLineEdit *port = new DecoratedLineEdit(topwidget, 150);
	DecoratedLineEdit *ipadress = new DecoratedLineEdit(topwidget, 150);
	Button *cancel = new Button(topwidget, tr("cancel"), std::bind(&Menu::showMainMenu, this));
	Button *connect = new Button(topwidget, tr("connect"), std::bind(&Menu::connect, this, ipadress->getString()));

	std::ostringstream oss;
	oss.str("");
	oss << Config::getInstance().connectto_tcpport;
	ipadress->setString(Config::getInstance().connectto_ip);
	port->setString(oss.str());

	//Set widgets position
	int interval = 100;
	sf::Vector2f size = topwidget->getSize();
	port->setPosition(size.x / 2.f- port->getSize().x / 2.f, size.y / 2.f- port->getSize().y / 2.f);
	ipadress->setPosition(size.x / 2.f - ipadress->getSize().x / 2.f, size.y / 2.f- ipadress->getSize().y / 2.f - interval);
	portlabel->setPosition(port->getPosition().x - portlabel->getSize().x - 25, port->getPosition().y + 10);
	iplabel->setPosition(ipadress->getPosition().x - iplabel->getSize().x - 25, ipadress->getPosition().y + 10);
	connect->setPosition(size.x / 3.f, size.y - interval);
	cancel->setPosition(2 * size.x / 3.f, size.y - interval);
}

void Menu::showHostMenu()
{
	m_guimgr.clear();
	Widget *topwidget = m_guimgr.getTopWidget();

	Label *portlabel = new Label(topwidget, tr("port"));
	DecoratedLineEdit *port = new DecoratedLineEdit(topwidget, 150);
	Button *cancel = new Button(topwidget, tr("cancel"), std::bind(&Menu::showMainMenu, this));
	Button *host = new Button(topwidget, tr("host"), std::bind(&Menu::host, this));

	std::ostringstream oss;
	oss.str("");
	oss << Config::getInstance().server_tcpport;
	port->setString(oss.str());

	//Set widgets position
	int interval = 100;
	sf::Vector2f size = topwidget->getSize();
	port->setPosition(size.x / 2.f- port->getSize().x / 2.f, size.y / 2.f- port->getSize().y / 2.f);
	portlabel->setPosition(port->getPosition().x - portlabel->getSize().x - 25, port->getPosition().y);
	host->setPosition(size.x / 3.f, size.y - interval);
	cancel->setPosition(2 * size.x / 3.f, size.y - interval);
}

void Menu::host()
{
	Config &config = Config::getInstance();
	std::cout << "Trying as server... " << std::endl;
	GameSimulator *simulator = new ServerSimulator(false);
	if(!static_cast<ServerSimulator *>(simulator)->loadMap("default"))
	{
		std::cout << "[Server failed]" << std::endl;
		delete simulator;
		simulator = nullptr;
	}
	else if(!static_cast<ServerSimulator *>(simulator)->startNetThread(config.server_tcpport, config.server_udpport, config.max_players))
	{
		std::cout << "[Server failed]" << std::endl;
		delete simulator;
		simulator = nullptr;
	}
	else
		std::cout << "[Server ok]" << std::endl;
	launchGame(simulator);
}

void Menu::connect(std::string ipadress)
{
	Config &config = Config::getInstance();
	config.connectto_ip = ipadress;
	config.save();
	GameSimulator *simulator = new ClientSimulator();
	int status;
	std::cout << "Trying as client..." << std::endl;
	if((status = static_cast<ClientSimulator *>(simulator)->startNetThread(sf::IpAddress(config.connectto_ip), config.connectto_tcpport, config.connectto_udpport, config.name)) != (int)ConnectionStatus::Accepted)
	{
		delete simulator;
		simulator = nullptr;
		if(status == (int)ConnectionStatus::GameIsFull)
		{
			std::cout << "[Game is full]" << std::endl;
		}
		else if(status == (int)ConnectionStatus::WrongAddress)
		{
			std::cout << "[Address/port error]" << std::endl;
		}
		else
		{
			std::cout << "[Client failed]" << std::endl;
		}
	}
	else
		std::cout << "[Client ok]" << std::endl;
	launchGame(simulator);
}

void Menu::launchGame(GameSimulator *simulator)
{
	if(simulator)
	{
		GameAppState *next = new GameAppState(m_window, m_vratio, m_xyratio, simulator);
		Application::getInstance().setNextAppState(next);
	}
}

/*void Menu::TEMPtestPlay()
{
	Config &config = Config::getInstance();
	GameSimulator *simulator = new ClientSimulator();
	int status;
	std::cout << "Trying as client..." << std::endl;
	if((status = static_cast<ClientSimulator *>(simulator)->startNetThread(sf::IpAddress(config.connectto_ip), config.connectto_tcpport, config.connectto_udpport, config.name)) != (int)ConnectionStatus::Accepted)
	{
		delete simulator;
		if(status == (int)ConnectionStatus::GameIsFull)
		{
			std::cout << "[Game is full]" << std::endl;
			simulator = nullptr;
		}
		else if(status == (int)ConnectionStatus::WrongAddress)
		{
			std::cout << "[Address/port error]" << std::endl;
			simulator = nullptr;
		}
		else
		{
			std::cout << "[Client failed]" << std::endl;
			std::cout << "Trying as server... " << std::endl;
			simulator = new ServerSimulator(false);
			if(!static_cast<ServerSimulator *>(simulator)->loadMap("default"))
			{
				std::cout << "[Server failed]" << std::endl;
				delete simulator;
				simulator = nullptr;
			}
			else if(!static_cast<ServerSimulator *>(simulator)->startNetThread(config.server_tcpport, config.server_udpport, config.max_players))
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
*/
