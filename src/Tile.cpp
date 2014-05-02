#include "Tile.hpp"

BinaryFile &operator>>(BinaryFile &file, Tile &tile)
{
	sf::Uint8 passable;
	file >> passable >> tile.appearance;
	tile.passable = passable;
	return file;
}

BinaryFile &operator<<(BinaryFile &file, const Tile &tile)
{
	file << tile.passable << tile.appearance;
	return file;
}
