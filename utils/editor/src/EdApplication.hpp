#ifndef EDAPPLICATION_HPP_INCLUDED
#define EDAPPLICATION_HPP_INCLUDED

#include <SFML/Graphics.hpp>

class EdApplication
{
	public:
	static EdApplication &getInstance();

	EdApplication(const EdApplication &) = delete;
	EdApplication &operator=(const EdApplication &) = delete;

	int execute(int argc, char **argv);

	private:
	EdApplication();
	sf::RenderWindow m_window;

	bool m_running;
};

#endif // EDAPPLICATION_HPP_INCLUDED
