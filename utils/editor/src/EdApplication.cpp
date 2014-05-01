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
	m_rect.top = 0;
	m_rect.left = 0;
	m_rect.width = 1024;
	m_rect.height = 768;

	sf::View view(m_rect);

	//Create the window
	m_window.setView(view);

	//Create the interface
	sf::RectangleShape frame(sf::Vector2f(200, 300));
	frame.setFillColor(sf::Color(94, 94, 94));
	frame.setOrigin(-50.f, -200.f);
	frame.setOutlineThickness(5);
	frame.setOutlineColor(sf::Color(60, 60, 60));

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
					m_guimgr.onKeyPressed(evt.key);
					if(evt.key.code == sf::Keyboard::Escape)
						m_running = false;
					else if(evt.key.code == sf::Keyboard::Up)
					{
						view.move(0, -1 * view.getSize().y / 2); //moves the camera up by half the height of the window
						m_rect.top -= view.getSize().y / 2;
						frame.move(0, -1 * view.getSize().y / 2);
					}
					else if(evt.key.code == sf::Keyboard::Down)
					{
						view.move(0, view.getSize().y / 2); //moves the camera down by half the height of the window
						m_rect.top += view.getSize().y / 2;
						frame.move(0, view.getSize().y / 2);
					}
					else if((evt.key.code == sf::Keyboard::Left)and(not(m_guimgr.onKeyPressed(evt.key))))
					{
						view.move(-1 * view.getSize().x / 2, 0); //moves the camera left by half the width of the window
						m_rect.left -= view.getSize().x / 2;
						frame.move(-1 * view.getSize().x / 2, 0);
					}
					else if((evt.key.code == sf::Keyboard::Right)and(not(m_guimgr.onKeyPressed(evt.key))))
					{
						view.move(view.getSize().x / 2, 0); //moves the camera right by half the width of the window
						m_rect.left += view.getSize().x / 2;
						frame.move(view.getSize().x / 2, 0);
					}
					break;

				case sf::Event::KeyReleased:
					m_guimgr.onKeyReleased(evt.key);
					break;

				case sf::Event::MouseWheelMoved:
					m_guimgr.onMouseWheelMoved(evt.mouseWheel);
					break;

				case sf::Event::MouseButtonPressed:
					m_guimgr.onMouseButtonPressed(evt.mouseButton);
					break;

				case sf::Event::MouseButtonReleased:
					m_guimgr.onMouseButtonReleased(evt.mouseButton);
					break;

				case sf::Event::MouseMoved:
					m_guimgr.onMouseMoved(evt.mouseMove);
					break;

				case sf::Event::TextEntered:
					m_guimgr.onTextEntered(evt.text);
					break;

				default:
					break;
			}
		}
		m_window.clear(sf::Color::Black);
		m_window.setView(view);
		m_dmap.draw(m_window, m_rect);
		m_window.draw(frame);
		m_guimgr.update(0.1f);
		m_window.setView(m_window.getDefaultView());
		m_guimgr.draw();
		m_window.display();
	}
	return 0;
}
