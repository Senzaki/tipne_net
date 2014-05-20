#ifndef GAMESCREEN_HPP_INCLUDED
#define GAMESCREEN_HPP_INCLUDED

#include "GameSimulator.hpp"
#include "DrawableMap.hpp"
#include "DrawableCharacter.hpp"

class GameScreen : public SimulatorStateListener
{
	public:
	GameScreen(float vratio, float xyratio, GameSimulator *simulator = nullptr);
	virtual ~GameScreen();

	GameScreen(const GameScreen &) = delete;
	GameScreen &operator=(const GameScreen &) = delete;

	void setSimulator(GameSimulator *simulator);

	bool update(float etime);
	void draw(sf::RenderWindow &window);

	void onKeyPressed(const sf::Event::KeyEvent &evt);
	void onKeyReleased(const sf::Event::KeyEvent &evt);
	void onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt);
	void onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt);
	void onMouseMoved(const sf::Event::MouseMoveEvent &evt);

	//Simulator events
	virtual void onNewPlayer(Player &player);
	virtual void onPlayerLeft(Player &player, sf::Uint8 reason);
	virtual void onNewEntity(GameEntity *entity);
	virtual void onEntityRemoved(GameEntity *entity);
	virtual void onMapLoaded(const Map &map);
	virtual void onVisibleEntitiesChanged(std::list<sf::Uint16> &&characters);

	private:
	void updateDirection();

	sf::View m_camera;//View for the drawables that NEED TO BE SCALED (e.g. images), but not the other ones (e.g. fonts)
	sf::FloatRect m_seen;
	float m_vratio;
	float m_xyratio;

	sf::Vector2f m_direction;
	sf::Vector2<bool> m_otherdirpressed;

	GameSimulator *m_simulator;
	DrawableMap m_map;

	std::unordered_map<sf::Uint16, DrawableCharacter> m_characters;
	std::list<sf::Uint16> m_visiblecharacters;
};

#endif // GAMESCREEN_HPP_INCLUDED
