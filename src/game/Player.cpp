#include "Player.hpp"

Player::Player(sf::Uint8 nid, const std::string &nname, bool nai):
	id(nid),
	name(nname),
	ai(nai)
{

}

Player::Player(Player &&other):
	id(other.id),
	name(std::move(other.name)),
	ai(other.ai)
{

}

Player &Player::operator=(Player &&other)
{
	id = other.id;
	name = std::move(other.name);
	ai = other.ai;

	return *this;
}

sf::Packet &operator<<(sf::Packet &packet, const Player &player)
{
	packet << player.id << player.name << player.ai;
	return packet;
}

sf::Packet &operator>>(sf::Packet &packet, Player &player)
{
	packet >> player.id >> player.name >> player.ai;
	return packet;
}
