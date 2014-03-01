#include "Tile.hpp"

sf::Packet &operator>>(sf::Packet &packet, Tile &tile)
{
	sf::Uint8 passable;
	packet >> passable >> tile.appearance;
	tile.passable = passable;
	return packet;
}
