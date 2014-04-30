#ifndef EDAPPLICATION_HPP_INCLUDED
#define EDAPPLICATION_HPP_INCLUDED

#include <SFML/Graphics.hpp>
#include "Map.hpp"
#include "DrawableMap.hpp"
#include "GUIManager.hpp"
#include "TileSettings.hpp"

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
	sf::FloatRect m_rect;
	Map m_map;
	DrawableMap m_dmap;
	GUIManager m_guimgr;
	bool m_running;
	TileSettings *m_tsettings;
};

#endif // EDAPPLICATION_HPP_INCLUDED
