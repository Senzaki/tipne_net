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
	Label *iplabel = new Label(topwidget, tr("ipadress"));
	DecoratedLineEdit *tcpport = new DecoratedLineEdit(topwidget, 150);
	DecoratedLineEdit *udpport = new DecoratedLineEdit(topwidget, 150);
	DecoratedLineEdit *ipadress = new DecoratedLineEdit(topwidget, 150);
	Button *cancel = new Button(topwidget, tr("cancel"), std::bind(&Menu::showMainMenu, this));
	Button *connect = new Button(topwidget, tr("connect"), std::bind(&Menu::connect, this, ipadress, tcpport, udpport));

	//Set default values in line edits
	ipadress->setString(Config::getInstance().connectto_ip);
	std::ostringstream oss;
	oss.str("");
	oss << Config::getInstance().connectto_tcpport;
	tcpport->setString(oss.str());
	oss.str("");
	oss << Config::getInstance().connectto_udpport;
	udpport->setString(oss.str());

	//Set widgets position
	constexpr int interval = 100;
	sf::Vector2f size = topwidget->getSize();
	ipadress->setPosition(size.x / 2.f - ipadress->getSize().x / 2.f, size.y / 2.f- ipadress->getSize().y / 2.f - interval);
	iplabel->setPosition(ipadress->getPosition().x - iplabel->getSize().x - 25, ipadress->getPosition().y + 10);
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
	DecoratedLineEdit *tcpport = new DecoratedLineEdit(topwidget, 150);
	DecoratedLineEdit *udpport = new DecoratedLineEdit(topwidget, 150);
	Button *cancel = new Button(topwidget, tr("cancel"), std::bind(&Menu::showMainMenu, this));
	Button *host = new Button(topwidget, tr("host"), std::bind(&Menu::host, this, tcpport, udpport));

	//Set default values in line edits
	std::ostringstream oss;
	oss.str("");
	oss << Config::getInstance().server_tcpport;
	tcpport->setString(oss.str());
	oss.str("");
	oss << Config::getInstance().server_udpport;
	udpport->setString(oss.str());

	//Set widgets position
	constexpr int interval = 100;
	sf::Vector2f size = topwidget->getSize();
	tcpport->setPosition(size.x / 2.f - tcpport->getSize().x / 2.f, size.y / 2.f - tcpport->getSize().y / 2.f - interval / 2.f);
	tcpportlabel->setPosition(tcpport->getPosition().x - tcpportlabel->getSize().x - 25, tcpport->getPosition().y);
	udpport->setPosition(size.x / 2.f - udpport->getSize().x / 2.f, size.y / 2.f - udpport->getSize().y / 2.f + interval / 2.f);
	udpportlabel->setPosition(udpport->getPosition().x - udpportlabel->getSize().x - 25, udpport->getPosition().y);
	host->setPosition(size.x / 3.f, size.y - interval);
	cancel->setPosition(2 * size.x / 3.f, size.y - interval);
}

void Menu::connect(DecoratedLineEdit* le_ipadress, DecoratedLineEdit* le_tcpport, DecoratedLineEdit* le_udpport)
{
	//TO DO :Comment
	Config &config = Config::getInstance();

	//Get strings
	std::string ipadress(le_ipadress->getString());
	std::string str_tcpport(le_tcpport->getString());
	std::string str_udpport(le_udpport->getString());

	//Convert string in short
	unsigned short tcpport;
	unsigned short udpport;
	convertPorts(str_tcpport, str_udpport, tcpport, udpport);

	GameSimulator *simulator = new ClientSimulator();
	int status;
	std::cout << "Trying as client..." << std::endl;
	if((status = static_cast<ClientSimulator *>(simulator)->startNetThread(sf::IpAddress(ipadress), tcpport, udpport, config.name)) != (int)ConnectionStatus::Accepted)
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

void Menu::host(DecoratedLineEdit* le_tcpport, DecoratedLineEdit* le_udpport)
{
	//TO DO : Comment
	Config &config = Config::getInstance();

	//Get strings
	std::string str_tcpport(le_tcpport->getString());
	std::string str_udpport(le_udpport->getString());

	//Convert string in short
	unsigned short tcpport;
	unsigned short udpport;
	convertPorts(str_tcpport, str_udpport, tcpport, udpport);

	std::cout << "Trying as server... " << std::endl;
	GameSimulator *simulator = new ServerSimulator(false);
	if(!static_cast<ServerSimulator *>(simulator)->loadMap("default"))
	{
		std::cout << "[Server failed]" << std::endl;
		delete simulator;
		simulator = nullptr;
	}
	else if(!static_cast<ServerSimulator *>(simulator)->startNetThread(tcpport, udpport, config.max_players))
	{
		std::cout << "[Server failed]" << std::endl;
		delete simulator;
		simulator = nullptr;
	}
	else
		std::cout << "[Server ok]" << std::endl;
	launchGame(simulator);
}

void Menu::convertPorts(const std::string &str_tcpport, const std::string &str_udpport, unsigned short &tcpport, unsigned short &udpport)
{
	std::istringstream iss;
	if(str_tcpport.empty())
		tcpport = DEFAULT_TCP_PORT;
	else
	{
		iss.str(str_tcpport);
		iss >> tcpport;
	}
	if(str_udpport.empty())
		udpport = DEFAULT_UDP_PORT;
	else
	{
		iss.str(str_udpport);
		iss >> udpport;
	}
}

void Menu::launchGame(GameSimulator *simulator)
{
	if(simulator)
	{
		GameAppState *next = new GameAppState(m_window, m_vratio, m_xyratio, simulator);
		Application::getInstance().setNextAppState(next);
	}
}

