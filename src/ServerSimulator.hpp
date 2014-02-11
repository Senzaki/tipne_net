#ifndef SERVERSIMULATOR_HPP_INCLUDED
#define SERVERSIMULATOR_HPP_INCLUDED

#include "GameSimulator.hpp"
#include "IDCreator.hpp"
#include <SFML/Network.hpp>
#include <list>
#include <thread>
#include <mutex>

class ServerSimulator : public GameSimulator
{
	public:
	ServerSimulator();
	virtual ~ServerSimulator();

	bool startNetThread(unsigned short port, sf::Uint8 maxplayers);
	void stopNetThread();

	void sendPacket(sf::Uint8 dest, sf::Packet &packet);

	private:
	void netThread();
	void acceptNewConnections(std::list<sf::TcpSocket *> &newclients, sf::SocketSelector &selector);
	bool receivePlayerConnectionInfo(sf::TcpSocket *socket, sf::SocketSelector &selector); //Returns false if no information was received (true if some data was received or if the client disconnected).
	bool receiveNewPacket(sf::TcpSocket *socket);

	sf::Socket::Status safeReceive(sf::TcpSocket *socket, sf::Packet &packet);
	sf::Socket::Status safeSend(sf::TcpSocket *socket, sf::Packet &packet);
	void safeDisconnect(sf::TcpSocket *socket, sf::Packet &packet);

	std::thread *m_thread;
	std::mutex m_netmutex;
	bool m_thrrunning;

	IDCreator<sf::Uint8> m_playersids;//Not locked
	std::unordered_map<std::string, int> m_names;//Not locked
	sf::Uint8 m_maxplayers;//Not locked

	sf::TcpListener m_listener;//Not locked
	std::unordered_map<sf::Uint8, sf::TcpSocket *> m_clients;//Don't write to the container in main thread. Mutex : m_netmutex.
};

#endif // SERVERSIMULATOR_HPP_INCLUDED
