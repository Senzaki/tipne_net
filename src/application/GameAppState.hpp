#ifndef GAMEAPPSTATE_HPP_INCLUDED
#define GAMEAPPSTATE_HPP_INCLUDED

#include "ApplicationState.hpp"
#include "GameSimulator.hpp"
#include "GUIManager.hpp"
#include "GameScreen.hpp"

class GameAppState : public ApplicationState
{
	public:
	GameAppState(sf::RenderWindow &window, float vratio, float xyratio, std::unique_ptr<GameSimulator> &&m_simulator);//The simulator will automatically be freed
	virtual ~GameAppState();

	GameAppState(const GameAppState &) = delete;
	GameAppState &operator=(const GameAppState &) = delete;

	virtual void load();
	virtual void update(float etime);
	virtual void draw();

	virtual void onWindowClosed();
	virtual void onFocusGained();
	virtual void onFocusLost();
	virtual void onKeyPressed(const sf::Event::KeyEvent &evt);
	virtual void onKeyReleased(const sf::Event::KeyEvent &evt);
	virtual void onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt);
	virtual void onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt);
	virtual void onMouseMoved(const sf::Event::MouseMoveEvent &evt);
	virtual void onTextEntered(const sf::Event::TextEvent &evt);

	private:
	void quit();

	sf::RenderWindow &m_window;
	std::unique_ptr<GameSimulator> m_simulator;
	GameScreen m_gscr;
	GUIManager m_guimgr;
	sf::View m_camera;//View for the drawables that NEED TO BE SCALED (e.g. images), but not the other ones (e.g. fonts)
	float m_vratio;
	float m_xyratio;
	sf::Sprite m_cursor;
};

#endif // GAMEAPPSTATE_HPP_INCLUDED
