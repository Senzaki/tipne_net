#ifndef APPLICATION_HPP_INCLUDED
#define APPLICATION_HPP_INCLUDED

#include <SFML/Graphics.hpp>
#include "ApplicationState.hpp"

#define DEFAULT_SCREEN_HEIGHT 1080.f //Default max resolution : 1920x1080 (16:9) -> but 1920x1200 won't display correctly :s

class Application
{
	public:
	static Application &getInstance();

	Application(const Application &) = delete;
	Application &operator=(const Application &) = delete;

	int execute(int argc, char **argv);

	void setNextAppState(ApplicationState *state, bool deleteold = true);

	private:
	Application();

	sf::RenderWindow m_window;

	ApplicationState *m_curstate;
	ApplicationState *m_nextstate;
	bool m_deletestate;
	bool m_running;
};

#endif // APPLICATION_HPP_INCLUDED
