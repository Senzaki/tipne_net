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
	RemoveCharacters,
	SetDirection
};

enum class DisconnectionReason : sf::Uint8
{
	Left = 0,
	Error,
	Kicked
};

const float DEFAULT_SNAPSHOT_TIME = 0.05f;
const float DEFAULT_INTERPOLATION_TIME = DEFAULT_SNAPSHOT_TIME * 2.f;

#endif // NETWORKCODES_HPP_INCLUDED
