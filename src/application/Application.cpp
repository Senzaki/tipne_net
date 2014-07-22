#include "Application.hpp"
#include "Config.hpp"
#include "Menu.hpp"
#include "Translator.hpp"
#include "ResourceManager.hpp"
#include "KeyMap.hpp"
#include <csignal>

static void quitApplication(int signal)
{
	Application::getInstance().setNextAppState(nullptr);
}

Application::Application():
	m_curstate(nullptr),
	m_nextstate(nullptr)
{

}

Application &Application::getInstance()
{
	static Application app;
	return app;
}

int Application::execute(int argc, char **argv)
{
	//Handle SIGINT (Ctrl + C) and SIGTERM (kill), so that the application can quit cleanly
	signal(SIGINT, &quitApplication);
	signal(SIGTERM, &quitApplication);

	const Config &conf = Config::getInstance();
	//Preload the key mapping
	KeyMap::getInstance();

	//Load base language package (for window title)
	Translator::getInstance().loadPackage("base");
	//Create the window
	m_window.create(sf::VideoMode(conf.width, conf.height), tr("window_title"), conf.fullscreen ? sf::Style::Fullscreen : sf::Style::Titlebar | sf::Style::Close);
	m_window.setVerticalSyncEnabled(conf.vsync);
	if(conf.dispfreq != 0)
		m_window.setFramerateLimit(conf.dispfreq);
	m_window.setMouseCursorVisible(false);
	m_window.setKeyRepeatEnabled(false);

	//Load base resources
	ResourceManager::getInstance().loadSection(ResourceSection::Base);

	//Let's start with the menu
	setNextAppState(std::make_shared<Menu>(m_window, m_window.getSize().y / DEFAULT_SCREEN_HEIGHT, (float)m_window.getSize().x / (float)m_window.getSize().y));
	sf::Event evt;
	sf::Clock clock;
	float etime;

	//Keep running while there's something to do (ie there's another state after the current one)
	while(m_nextstate)
	{
		//Set the new state as the current state
		m_curstate = m_nextstate;
		m_nextstate = nullptr;
		m_running = true;

		m_curstate->load();

		//Enter the main loop for this state
		while(m_running)
		{
			//Measure elapsed time
			etime = clock.getElapsedTime().asSeconds();
			clock.restart();
			//Iterate through all events
			while(m_window.pollEvent(evt))
			{
				switch(evt.type)
				{
					case sf::Event::Closed:
						m_curstate->onWindowClosed();
						break;

					case sf::Event::TextEntered:
						m_curstate->onTextEntered(evt.text);
						break;

					case sf::Event::KeyPressed:
						m_curstate->onKeyPressed(evt.key);
						break;

					case sf::Event::KeyReleased:
						m_curstate->onKeyReleased(evt.key);
						break;

					case sf::Event::MouseWheelMoved:
						m_curstate->onMouseWheelMoved(evt.mouseWheel);
						break;

					case sf::Event::MouseButtonPressed:
						m_curstate->onMouseButtonPressed(evt.mouseButton);
						break;

					case sf::Event::MouseButtonReleased:
						m_curstate->onMouseButtonReleased(evt.mouseButton);
						break;

					case sf::Event::MouseMoved:
						m_curstate->onMouseMoved(evt.mouseMove);
						break;

					case sf::Event::LostFocus:
						m_curstate->onFocusLost();
						break;

					case sf::Event::GainedFocus:
						m_curstate->onFocusGained();
						break;

					default:
						break;
				}
			}
			//Update & draw everything
			m_curstate->update(etime);
			m_curstate->draw();
			m_window.display();
		}
	}

	return 0;
}

void Application::setNextAppState(const std::shared_ptr<ApplicationState> &state)
{
	m_nextstate = state;
	m_running = false;
}
