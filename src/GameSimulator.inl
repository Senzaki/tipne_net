#include <iostream>

template<typename... Args>
inline Player *GameSimulator::addPlayer(Args &&...args)
{
	return addPlayer(Player(std::forward<Args>(args)...));
}

template<typename... Args>
inline Character *GameSimulator::addCharacter(Args &&...args)
{
	return addCharacter(Character(std::forward<Args>(args)...));
}
