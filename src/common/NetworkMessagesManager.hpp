#ifndef NETWORKMESSAGESMANAGER_HPP_INCLUDED
#define NETWORKMESSAGESMANAGER_HPP_INCLUDED

#include "SafeSocket.hpp"
#include <unordered_map>
#include <list>

class NetworkMessagesManager
{
	public:
	NetworkMessagesManager();

	NetworkMessagesManager(const NetworkMessagesManager &) = delete;
	NetworkMessagesManager &operator=(const NetworkMessagesManager &) = delete;

	void addSocket(sf::Uint8 id, SafeSocket<sf::TcpSocket> &socket);
	void removeSocket(sf::Uint8 id);
	sf::Uint32 getSequenceNumber(sf::Uint8 id) const;

	template<typename T>
	NetworkMessagesManager &operator<<(T &&data);
	void append(const sf::Packet &packet);
	sf::Packet &getIndividualPacket(sf::Uint8 id);//Will split the packets, less performance

	void sendMessages(std::list<std::pair<sf::Uint8, sf::Socket::Status>> &errors);

	private:
	void split();

	bool m_split;
	sf::Packet m_general;

	struct MessageReceiver
	{
		MessageReceiver(SafeSocket<sf::TcpSocket> *nsocket):
			socket(nsocket),
			seqnbr(0)
		{
		}

		SafeSocket<sf::TcpSocket> *socket;
		sf::Packet packet;
		unsigned int seqnbr;
	};

	std::unordered_map<sf::Uint8, MessageReceiver> m_receivers;
};

#include "NetworkMessagesManager.inl"

#endif // NETWORKMESSAGESMANAGER_HPP_INCLUDED
