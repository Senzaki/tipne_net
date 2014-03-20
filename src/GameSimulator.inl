#include <iostream>

template<typename... Args>
inline bool GameSimulator::addPlayer(Args &&...args)
{
	return addPlayer(Player(std::forward<Args>(args)...));
}

template<typename... Args>
inline bool GameSimulator::addCharacter(Args &&...args)
{
	return addCharacter(Character(std::forward<Args>(args)...));
}
