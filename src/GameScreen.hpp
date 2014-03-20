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

	//Simulator events
	virtual void onNewPlayer(Player &player);
	virtual void onPlayerLeft(Player &player, sf::Uint8 reason);
	virtual void onNewCharacter(Character &character);
	virtual void onCharacterRemoved(Character &character);
	virtual void onMapLoaded(const Map &map);

	private:
	sf::View m_camera;//View for the drawables that NEED TO BE SCALED (e.g. images), but not the other ones (e.g. fonts)
	sf::FloatRect m_seen;
	float m_vratio;
	float m_xyratio;

	GameSimulator *m_simulator;
	DrawableMap m_map;

	std::unordered_map<sf::Uint16, DrawableCharacter> m_characters;
};

#endif // GAMESCREEN_HPP_INCLUDED
