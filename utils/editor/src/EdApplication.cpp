#include "EdApplication.hpp"
#include "Map.hpp"
#include "DrawableMap.hpp"
#include "ResourceManager.hpp"
#include <SFML/Graphics.hpp>
//#include "BasisChange.hpp"

EdApplication::EdApplication() : m_window(sf::VideoMode(1024, 768), "Editor Window", sf::Style::Close), m_guimgr(m_window)
{
	ResourceManager::getInstance().loadSection(ResourceSection::Base);
	ResourceManager::getInstance().loadSection(ResourceSection::Map);
}

EdApplication &EdApplication::getInstance()
{
	static EdApplication app;
	return app;
}

int EdApplication::execute(int argc, char **argv)
{
	sf::Event evt;
	m_running = true;
	m_map.load("default");
	m_dmap.setMap(m_map);
	m_tsettings = new TileSettings(m_guimgr.getTopWidget());

	//Create the view
	sf::View view(sf::FloatRect(0,0,1024,768));

	//Create the window
	m_window.setView(view);

	while(m_running)
	{
		//Wait for events
		while(m_window.pollEvent(evt))
		{
			switch(evt.type)
			{
				case sf::Event::Closed:
					m_running = false;
					break;

				case sf::Event::KeyPressed:
					if(evt.key.code == sf::Keyboard::Escape)
						m_running = false;
					else if(evt.key.code == sf::Keyboard::Up)
						view.move(0, -1 * view.getSize().y / 2); //moves the camera up by half the height of the window
					else if(evt.key.code == sf::Keyboard::Down)
						view.move(0, view.getSize().y / 2); //moves the camera down by half the height of the window
					else if(evt.key.code == sf::Keyboard::Left)
						view.move(-1 * view.getSize().x / 2, 0); //moves the camera left by half the width of the window
					else if(evt.key.code == sf::Keyboard::Right)
						view.move(view.getSize().x / 2, 0); //moves the camera right by half the width of the window
					break;

				case sf::Event::MouseButtonPressed:
					break;

				default:
					break;
			}
		}
		m_window.clear(sf::Color::Black);
		m_window.setView(view);
		m_dmap.draw(m_window, sf::FloatRect(0,0,1024,768));
		m_guimgr.update(0.1f);
		m_window.setView(m_window.getDefaultView());
		m_guimgr.draw();
		m_window.display();
	}
	return 0;
}
