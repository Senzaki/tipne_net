#ifndef CLIENTSIMULATOR_HPP_INCLUDED
#define CLIENTSIMULATOR_HPP_INCLUDED

#include "GameSimulator.hpp"
#include <thread>
#include "SafeSocket.hpp"
#include <list>

class ClientSimulator : public GameSimulator
{
	public:
	ClientSimulator();
	virtual ~ClientSimulator();

	virtual void update(float etime);

	int startNetThread(const sf::IpAddress &serveraddr, unsigned short port, const std::string &name); //Returns -1 in case of error, and the connection status otherwise
	void stopNetThread();

	private:
	bool parseConnectionData(sf::Packet &packet);
	bool onNewPlayerPacket(sf::Packet &packet);
	bool onDisconnectionPacket(sf::Packet &packet);

	void netThread();
	bool receivePackets();

	std::thread *m_thread;
	bool m_thrrunning;

	SafeSocket m_server;
	std::list<sf::Packet> m_receivedpackets;//Write : child. Read : main.
	std::mutex m_receivemutex;
};

#endif // CLIENTSIMULATOR_HPP_INCLUDED
