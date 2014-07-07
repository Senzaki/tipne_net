#ifndef TILE_HPP_INCLUDED
#define TILE_HPP_INCLUDED

#include "BinaryFile.hpp"

static constexpr sf::Uint16 FIRST_WALL_APPEARANCE = 0x100;

struct Tile
{
	bool passable;
	sf::Uint16 appearance;

	static constexpr std::size_t getSizeInPacket()
	{
		return sizeof(sf::Uint8)/*passable*/ + sizeof(sf::Uint16)/*appearance*/;
	}

	bool operator==(const Tile &other) const
	{
		return other.passable == passable && other.appearance == appearance;
	}

	bool operator!=(const Tile &other) const
	{
		return !(*this == other);
	}
};

BinaryFile &operator>>(BinaryFile &file, Tile &tile);
BinaryFile &operator<<(BinaryFile &file, const Tile &tile);

#endif // TILE_HPP_INCLUDED
