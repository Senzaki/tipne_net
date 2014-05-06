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

	//Create widgets
	Label *tcpportlabel = new Label(topwidget, tr("tcpport"));
	Label *udpportlabel = new Label(topwidget, tr("udpport"));
	Label *iplabel = new Label(topwidget, tr("ipaddress"));
	DecoratedLineEdit *tcpport = new DecoratedLineEdit(topwidget, 150);
	DecoratedLineEdit *udpport = new DecoratedLineEdit(topwidget, 150);
	DecoratedLineEdit *ipaddress = new DecoratedLineEdit(topwidget, 150);
	Button *cancel = new Button(topwidget, tr("cancel"), std::bind(&Menu::showMainMenu, this));
	Button *connect = new Button(topwidget, tr("connect"), std::bind(&Menu::connect, this, ipaddress, tcpport, udpport));

	//Set default values in line edits
	if(Config::getInstance().connectto_tcpport != DEFAULT_TCP_PORT)
	{
		std::ostringstream oss;
		oss << Config::getInstance().connectto_tcpport;
		tcpport->setString(oss.str());
	}
	if(Config::getInstance().connectto_udpport != DEFAULT_UDP_PORT)
	{
		std::ostringstream oss;
		oss << Config::getInstance().connectto_udpport;
		udpport->setString(oss.str());
	}
	ipaddress->setString(Config::getInstance().connectto_ip);

	//Set widgets position
	constexpr int interval = 100;
	sf::Vector2f size = topwidget->getSize();
	ipaddress->setPosition(size.x / 2.f - ipaddress->getSize().x / 2.f, size.y / 2.f- ipaddress->getSize().y / 2.f - interval);
	iplabel->setPosition(ipaddress->getPosition().x - iplabel->getSize().x - 25, ipaddress->getPosition().y + 10);
	tcpport->setPosition(size.x / 2.f - tcpport->getSize().x / 2.f, size.y / 2.f - tcpport->getSize().y / 2.f);
	tcpportlabel->setPosition(tcpport->getPosition().x - tcpportlabel->getSize().x - 25, tcpport->getPosition().y + 10);
	udpport->setPosition(size.x / 2.f - udpport->getSize().x / 2.f, size.y / 2.f - udpport->getSize().y / 2.f + interval);
	udpportlabel->setPosition(udpport->getPosition().x - udpportlabel->getSize().x - 25, udpport->getPosition().y + 10);
	connect->setPosition(size.x / 3.f, size.y - interval);
	cancel->setPosition(2 * size.x / 3.f, size.y - interval);
}

void Menu::showHostMenu()
{
	m_guimgr.clear();
	Widget *topwidget = m_guimgr.getTopWidget();

	//Create widgets
	Label *tcpportlabel = new Label(topwidget, tr("tcpport"));
	Label *udpportlabel = new Label(topwidget, tr("udpport"));
	Label *maxplayerslabel = new Label(topwidget, tr("maxplayers"));
	DecoratedLineEdit *tcpport = new DecoratedLineEdit(topwidget, 150);
	DecoratedLineEdit *udpport = new DecoratedLineEdit(topwidget, 150);
	DecoratedLineEdit *maxplayers = new DecoratedLineEdit(topwidget, 150);
	Button *cancel = new Button(topwidget, tr("cancel"), std::bind(&Menu::showMainMenu, this));
	Button *host = new Button(topwidget, tr("host"), std::bind(&Menu::host, this, tcpport, udpport, maxplayers));

	//Set default values in line edits
	if(Config::getInstance().server_tcpport != DEFAULT_TCP_PORT)
	{
		std::ostringstream oss;
		oss << Config::getInstance().server_tcpport;
		tcpport->setString(oss.str());
	}
	if(Config::getInstance().server_udpport != DEFAULT_UDP_PORT)
	{
		std::ostringstream oss;
		oss << Config::getInstance().server_udpport;
		udpport->setString(oss.str());
	}
	std::ostringstream oss;
	oss << (int)Config::getInstance().max_players;
	maxplayers->setString(oss.str());

	//Set widgets position
	constexpr int interval = 100;
	sf::Vector2f size = topwidget->getSize();
	tcpport->setPosition(size.x / 2.f - tcpport->getSize().x / 2.f, size.y / 2.f - tcpport->getSize().y / 2.f - interval);
	tcpportlabel->setPosition(tcpport->getPosition().x - tcpportlabel->getSize().x - 25, tcpport->getPosition().y);
	udpport->setPosition(size.x / 2.f - udpport->getSize().x / 2.f, size.y / 2.f - udpport->getSize().y / 2.f);
	udpportlabel->setPosition(udpport->getPosition().x - udpportlabel->getSize().x - 25, udpport->getPosition().y);
	maxplayers->setPosition(size.x / 2.f - maxplayers->getSize().x / 2.f, size.y / 2.f - maxplayers->getSize().y / 2.f + interval);
	maxplayerslabel->setPosition(maxplayers->getPosition().x - maxplayerslabel->getSize().x - 25, maxplayers->getPosition().y);
	host->setPosition(size.x / 3.f, size.y - interval);
	cancel->setPosition(2 * size.x / 3.f, size.y - interval);
}

void Menu::connect(DecoratedLineEdit *ipaddrwidget, DecoratedLineEdit *tcpportwidget, DecoratedLineEdit *udpportwidget)
{
	Config &config = Config::getInstance();

	//Get strings
	sf::IpAddress ipaddress(ipaddrwidget->getString());
	if(ipaddress == sf::IpAddress::None)
	{
		std::cerr << "Invalid server address." << std::endl;
		return;
	}
	std::string tcpportstr(tcpportwidget->getString());
	std::string udpportstr(udpportwidget->getString());

	//Convert ports string unsigned short
	unsigned short tcpport;
	unsigned short udpport;
	if(!convertPorts(tcpportstr, udpportstr, tcpport, udpport))
	{
		std::cerr << "Invalid port number." << std::endl;
		return;
	}

	//Save the config
	Config::getInstance().connectto_ip = ipaddress.toString();
	Config::getInstance().connectto_tcpport = tcpport;
	Config::getInstance().connectto_udpport = udpport;
	Config::getInstance().save();

	GameSimulator *simulator = new ClientSimulator();
	int status;
	//Launch simulator
	if((status = static_cast<ClientSimulator *>(simulator)->startNetThread(ipaddress, tcpport, udpport, config.name)) != (int)ConnectionStatus::Accepted)
	{
		delete simulator;
		if(status == (int)ConnectionStatus::GameIsFull)
			std::cerr << "Unable to join game : game is full." << std::endl;
		else if(status == (int)ConnectionStatus::WrongAddress)
			std::cerr << "Unable to join game : address/port error." << std::endl;
		else
			std::cout << "Unable to join game." << std::endl;
		return;
	}

	launchGame(simulator);
}

void Menu::host(DecoratedLineEdit *tcpportwidget, DecoratedLineEdit *udpportwidget, DecoratedLineEdit *maxplayerswidget)
{
	Config &config = Config::getInstance();

	//Get strings
	std::string tcpportstr(tcpportwidget->getString());
	std::string udpportstr(udpportwidget->getString());

	//Convert ports string to unsigned short
	unsigned short tcpport;
	unsigned short udpport;
	if(!convertPorts(tcpportstr, udpportstr, tcpport, udpport))
	{
		std::cerr << "Invalid port number." << std::endl;
		return;
	}

	//Convert max players strings to uint
	unsigned int maxplayers(0);
	std::istringstream iss;
	iss.str(maxplayerswidget->getString());
	if(!(iss >> maxplayers) || maxplayers > 254 || maxplayers < 1)
	{
		std::cerr << "Invalid max players number" << std::endl;
		return;
	}

	//Save the config
	Config::getInstance().max_players = maxplayers;
	Config::getInstance().server_tcpport = tcpport;
	Config::getInstance().server_udpport = udpport;
	Config::getInstance().save();

	GameSimulator *simulator = new ServerSimulator(false);
	//Load map
	if(!static_cast<ServerSimulator *>(simulator)->loadMap("default"))
	{
		std::cerr << "Unable to host game : map cannot be loaded." << std::endl;
		delete simulator;
		return;
	}
	//Launch simulator
	if(!static_cast<ServerSimulator *>(simulator)->startNetThread(tcpport, udpport, config.max_players))
	{
		std::cerr << "Unable to host game." << std::endl;
		delete simulator;
		return;
	}
	launchGame(simulator);
}

bool Menu::convertPorts(const std::string &tcpportstr, const std::string &udpportstr, unsigned short &tcpport, unsigned short &udpport)
{
	std::istringstream iss;
	//Default port if no port specified
	if(tcpportstr.empty())
		tcpport = DEFAULT_TCP_PORT;
	else
	{
		//Extract port
		iss.str(tcpportstr);
		if(!(iss >> tcpport))
			return false;
	}
	//Default port if no port specified
	if(udpportstr.empty())
		udpport = DEFAULT_UDP_PORT;
	else
	{
		//Extract port
		iss.clear();
		iss.str(udpportstr);
		if(!(iss >> udpport))
			return false;
	}
	return true;
}

void Menu::launchGame(GameSimulator *simulator)
{
	GameAppState *next = new GameAppState(m_window, m_vratio, m_xyratio, simulator);
	Application::getInstance().setNextAppState(next);
}
