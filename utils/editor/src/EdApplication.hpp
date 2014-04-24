#ifndef EDAPPLICATION_HPP_INCLUDED
#define EDAPPLICATION_HPP_INCLUDED

#include <SFML/Graphics.hpp>
#include "Map.hpp"
#include "DrawableMap.hpp"

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
	Map m_map;
	DrawableMap m_dmap;

	bool m_running;
};

#endif // EDAPPLICATION_HPP_INCLUDED
