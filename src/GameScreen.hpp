#ifndef GAMESCREEN_HPP_INCLUDED
#define GAMESCREEN_HPP_INCLUDED

#include "ApplicationState.hpp"
#include "GameSimulator.hpp"
#include "GUIManager.hpp"

class GameScreen : public ApplicationState
{
	public:
	GameScreen(sf::RenderWindow &window, float vratio, float xyratio, GameSimulator *m_simulator);//The simulator will automatically be freed
	virtual ~GameScreen();

	GameScreen(const GameScreen &) = delete;
	GameScreen &operator=(const GameScreen &) = delete;

	virtual void load();
	virtual void update(float etime);
	virtual void draw();

	virtual void onWindowClosed();
	virtual void onKeyPressed(const sf::Event::KeyEvent &evt);
	virtual void onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt);
	virtual void onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt);
	virtual void onMouseMoved(const sf::Event::MouseMoveEvent &evt);
	virtual void onTextEntered(const sf::Event::TextEvent &evt);

	private:
	sf::RenderWindow &m_window;
	GameSimulator *m_simulator;
	GUIManager m_guimgr;
	sf::View m_camera;//View for the drawables that NEED TO BE SCALED (e.g. images), but not the other ones (e.g. fonts)
	float m_vratio;
	float m_xyratio;
	sf::Sprite m_cursor;
};

#endif // GAMESCREEN_HPP_INCLUDED
