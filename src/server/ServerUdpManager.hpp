#ifndef SERVERUDPMANAGER_HPP_INCLUDED
#define SERVERUDPMANAGER_HPP_INCLUDED

#include "SafeSocket.hpp"
#include <thread>
#include <atomic>
#include "SafeList.hpp"
#include <unordered_map>
class ServerSimulator;

class ServerUdpManager
{
	public:
	ServerUdpManager(ServerSimulator &simulator);
	~ServerUdpManager();

	void update(float etime);

	bool startNetThread(unsigned short udpport);
	void stopNetThread();

	bool addPlayer(sf::Uint8 id, sf::IpAddress addr, unsigned short port);
	void removePlayer(sf::Uint8 id);

	private:
	void netThread();
	void receiveNewPackets();

	sf::Uint8 getAssociatedPlayer(const sf::IpAddress &address, unsigned short port);
	void parseReceivedPacket(sf::Packet &packet, sf::Uint8 id);
	void sendToAll(sf::Packet &packet);

	std::unique_ptr<std::thread> m_thread;
	std::atomic<bool> m_thrrunning;
	ServerSimulator &m_simulator;

	SafeSocket<sf::UdpSocket> m_socket;
	struct PlayerInfo
	{
		sf::IpAddress address;
		unsigned short port;

		bool operator==(const PlayerInfo &other) const;
	};
	template<int hashsize>
	static size_t hashPlayerInfo(const PlayerInfo &info);

	std::unordered_map<PlayerInfo, sf::Uint8, size_t (*)(const PlayerInfo &)> m_playersinfo;
	std::unordered_map<sf::Uint8, float> m_lastpacketreceived;
	float m_lastsnapshot;

	SafeList<std::tuple<sf::IpAddress, unsigned short, std::unique_ptr<sf::Packet>>> m_receivedpackets;
};

#endif // SERVERUDPMANAGER_HPP_INCLUDED
