#include "EdApplication.hpp"
#include "Map.hpp"
#include "DrawableMap.hpp"
#include "ResourceManager.hpp"
#include <SFML/Graphics.hpp>
#include "BasisChange.hpp"
#include <iostream>

EdApplication::EdApplication():
	m_window(sf::VideoMode(1550, 850), "Editor Window", sf::Style::Close),
	m_guimgr(m_window),
	m_pasting(false)
{
	ResourceManager::getInstance().loadSection(ResourceSection::Base);
	ResourceManager::getInstance().loadSection(ResourceSection::Map);
	m_window.setFramerateLimit(60);
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

	//Load the map
	std::string loadname;
	std::cout << "Write the name of the map you want to open :" << std::endl;
	std::cin >> loadname;
	m_map.load(loadname);
	m_dmap.setMap(m_map);

	m_tsettings = new TileSettings(m_guimgr.getTopWidget());
	sf::Vector2f tilecoords(0.f, 0.f);

	//Create the view
	m_rect.top = 0;
	m_rect.left = 0;
	m_rect.width = 1550;
	m_rect.height = 850;

	sf::View view(m_rect);

	//Create the window
	m_window.setView(view);

	//Create the interface
	sf::RectangleShape frame(sf::Vector2f(200, 300));
	frame.setFillColor(sf::Color(60, 60, 60));
	frame.setOrigin(-50.f, -200.f);
	frame.setOutlineThickness(5);
	frame.setOutlineColor(sf::Color(40, 40, 40));

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
					if(not(m_guimgr.onKeyPressed(evt.key)))
					{
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
						else if(evt.key.code == sf::Keyboard::Left)
						{
							view.move(-1 * view.getSize().x / 2, 0); //moves the camera left by half the width of the window
							m_rect.left -= view.getSize().x / 2;
							frame.move(-1 * view.getSize().x / 2, 0);
						}
						else if(evt.key.code == sf::Keyboard::Right)
						{
							view.move(view.getSize().x / 2, 0); //moves the camera right by half the width of the window
							m_rect.left += view.getSize().x / 2;
							frame.move(view.getSize().x / 2, 0);
						}
						else if(evt.key.code == sf::Keyboard::Return)
						{
							m_map.setTile(tilecoords.x, tilecoords.y, m_tsettings->getUpdatedTile());
							m_dmap.setMap(m_map);
						}
						else if(evt.key.code == sf::Keyboard::S)
						{
							std::string savename(" ");
							std::cout << "Write a name for this map :" << std::endl;
							std::cin >> savename;
							m_map.save(savename);
						}
					}
					break;

				case sf::Event::KeyReleased:
					m_guimgr.onKeyReleased(evt.key);
					break;

				case sf::Event::MouseWheelMoved:
					m_guimgr.onMouseWheelMoved(evt.mouseWheel);
					break;

				case sf::Event::MouseButtonPressed:
					if(!m_guimgr.onMouseButtonPressed(evt.mouseButton))
					{
						if(evt.mouseButton.button == sf::Mouse::Left)
						{
							if((BasisChange::pixelToGrid(evt.mouseButton.x + m_rect.left, evt.mouseButton.y + m_rect.top).x > 0) && (BasisChange::pixelToGrid(evt.mouseButton.x + m_rect.left, evt.mouseButton.y + m_rect.top).y > 0)) //This condition is to be sure the user clicked on a tile
							{
								tilecoords = BasisChange::pixelToGrid(evt.mouseButton.x + m_rect.left, evt.mouseButton.y + m_rect.top);
								m_tsettings->setTile(m_map.getTile(tilecoords.x, tilecoords.y)); //Will show the informations about the selected tile
							}
						}
						else if(evt.mouseButton.button == sf::Mouse::Right)
							m_pasting = true;
					}
					break;

				case sf::Event::MouseButtonReleased:
					if(!m_guimgr.onMouseButtonReleased(evt.mouseButton))
					{
						if(evt.mouseButton.button == sf::Mouse::Right)
							m_pasting = false;
					}
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

		if(m_pasting)
		{
			tilecoords = BasisChange::pixelToGrid(sf::Mouse::getPosition(m_window).x + m_rect.left, sf::Mouse::getPosition(m_window).y + m_rect.top);
			if(m_map.getTile(tilecoords.x, tilecoords.y) != m_tsettings->getUpdatedTile())
			{
				m_map.setTile(tilecoords.x, tilecoords.y, m_tsettings->getUpdatedTile());
				m_dmap.setMap(m_map);
			}
		}

		m_window.clear(sf::Color::Black);
		m_window.setView(view);

		std::list<DrawableEntity *> todraw;
		//Draw the map & add map entities to thez list
		m_dmap.draw(m_window, m_rect, todraw);
		//Sort the entities by depth
		todraw.sort(DrawableEntity::isDepthLower);
		//Draw all the entities
		for(DrawableEntity *entity : todraw)
			entity->draw(m_window);

		m_window.draw(frame);
		m_guimgr.update(0.1f);
		m_window.setView(m_window.getDefaultView());
		m_guimgr.draw();
		m_window.display();
	}
	return 0;
}
