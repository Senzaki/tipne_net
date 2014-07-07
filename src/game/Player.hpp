#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED

#include <SFML/Network.hpp>

const sf::Uint8 NEUTRAL_PLAYER = 0;

struct Player
{
	Player(sf::Uint8 nid = NEUTRAL_PLAYER, const std::string &nname = std::string(), bool nai = false);
	Player(Player &&other);
	Player &operator=(Player &&other);

	Player(const Player &) = delete;
	Player &operator=(const Player &) = delete;

	sf::Uint8 id;
	std::string name;
	bool ai;
};

sf::Packet &operator<<(sf::Packet &packet, const Player &player);
sf::Packet &operator>>(sf::Packet &packet, Player &player);

#endif // PLAYER_HPP_INCLUDED
