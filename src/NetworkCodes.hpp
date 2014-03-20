#ifndef NETWORKCODES_HPP_INCLUDED
#define NETWORKCODES_HPP_INCLUDED

enum class ConnectionStatus : sf::Uint8
{
	Accepted = 0,
	GameIsFull
};

enum class PacketType : sf::Uint8
{
	NewPlayer = 0,
	Disconnection,
	Map,
	NewCharacter,
	RemoveCharacters
};

enum class DisconnectionReason : sf::Uint8
{
	Left = 0,
	Error,
	Kicked
};

#endif // NETWORKCODES_HPP_INCLUDED
