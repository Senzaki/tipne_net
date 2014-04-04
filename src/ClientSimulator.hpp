#ifndef CLIENTSIMULATOR_HPP_INCLUDED
#define CLIENTSIMULATOR_HPP_INCLUDED

#include "GameSimulator.hpp"
#include <thread>
#include <atomic>
#include "SafeSocket.hpp"
#include "SafeList.hpp"

class ClientSimulator : public GameSimulator
{
	public:
	ClientSimulator();
	virtual ~ClientSimulator();

	virtual bool update(float etime);

	int startNetThread(const sf::IpAddress &serveraddr, unsigned short port, const std::string &name); //Returns -1 in case of error, and the connection status otherwise
	void stopNetThread();
	bool isConnected() const;

	virtual void selfSetDirection(const sf::Vector2f &direction);

	private:
	bool parseConnectionData(sf::Packet &packet);
	bool parseReceivedPacket(sf::Packet &packet);
	bool onNewPlayerPacket(sf::Packet &packet);
	bool onDisconnectionPacket(sf::Packet &packet);
	bool onMapPacket(sf::Packet &packet);
	bool onNewCharacterPacket(sf::Packet &packet);
	bool onRemoveCharactersPacket(sf::Packet &packet);
	bool onSetDirectionPacket(sf::Packet &packet);

	void netThread();
	bool receivePackets();

	std::thread *m_thread;
	std::atomic<bool> m_thrrunning;

	SafeSocket m_server;
	SafeList<sf::Packet *> m_receivedpackets;//Write : child. Read : main.
};

#endif // CLIENTSIMULATOR_HPP_INCLUDED
