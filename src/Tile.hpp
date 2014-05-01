#ifndef TILE_HPP_INCLUDED
#define TILE_HPP_INCLUDED

#include "SFML/Network.hpp"

struct Tile
{
	bool passable;
	sf::Uint16 appearance;

	static constexpr std::size_t getSizeInPacket()
	{
		return sizeof(sf::Uint8)/*passable*/ + sizeof(sf::Uint16)/*appearance*/;
	}
};

sf::Packet &operator>>(sf::Packet &packet, Tile &tile);
sf::Packet &operator<<(sf::Packet &packet, const Tile &tile);

#endif // TILE_HPP_INCLUDED
