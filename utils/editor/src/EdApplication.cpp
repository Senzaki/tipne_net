#include "EdApplication.hpp"
//#include "BasisChange.hpp"

EdApplication::EdApplication()
{

}

EdApplication &EdApplication::getInstance()
{
	static EdApplication app;
	return app;
}

int EdApplication::execute(int argc, char **argv)
{
	sf::Event evt;
	sf::Clock clock;
	float etime;

	m_running = true;

	//Create the window
	m_window.create(sf::VideoMode(1027, 768), "Editor Window", sf::Style::Close);

	while(m_running)
		{
			etime = clock.getElapsedTime().asSeconds();
			clock.restart();

			while(m_window.pollEvent(evt))
			{
				switch(evt.type)
				{
					case sf::Event::Closed:
						m_running = false;
						break;

					default:
						break;
				};
			};
			m_window.display();
		};
	return 0;
}
