#include "make_unique.hpp"

template<typename... Args>
inline Player *GameSimulator::addPlayer(Args &&...args)
{
	return addPlayer(Player(std::forward<Args>(args)...));
}
