#ifndef CLIENTUDPMANAGER_HPP_INCLUDED
#define CLIENTUDPMANAGER_HPP_INCLUDED

#include "SafeSocket.hpp"
#include "SafeList.hpp"
#include <atomic>
class ClientSimulator;

class ClientUdpManager
{
	public:
	ClientUdpManager(ClientSimulator &simulator);
	~ClientUdpManager();

	bool update(float etime);//Returns false on connection timeout

	ClientUdpManager(const ClientUdpManager &) = delete;
	ClientUdpManager &operator=(const ClientUdpManager &) = delete;

	unsigned short startNetThread(sf::IpAddress addr, unsigned short udpport);//Returns the local port on success, and 0 otherwise
	void stopNetThread();

	private:
	void netThread();
	bool receiveNewPackets();

	void sendPacket(sf::Packet &packet);//Use it to reset the lastpacketsent counter
	bool parseReceivedPacket(sf::Packet &packet);

	std::unique_ptr<std::thread> m_thread;
	std::atomic<bool> m_thrrunning;
	ClientSimulator &m_simulator;

	SafeSocket<sf::UdpSocket> m_server;
	float m_lastpacketreceived;
	float m_lastpacketsent;
	float m_keepaliveinterval;

	//Don't use these in main thread
	sf::IpAddress m_address;
	unsigned short m_port;

	SafeList<std::unique_ptr<sf::Packet>> m_receivedpackets;
};

#endif // CLIENTUDPMANAGER_HPP_INCLUDED
