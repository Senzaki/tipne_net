#ifndef SERVERSIMULATOR_HPP_INCLUDED
#define SERVERSIMULATOR_HPP_INCLUDED

#include "GameSimulator.hpp"
#include "IDCreator.hpp"
#include "SafeSocket.hpp"
#include <list>
#include <thread>

class ServerSimulator : public GameSimulator
{
	public:
	ServerSimulator(bool pure);
	virtual ~ServerSimulator();

	virtual void update(float etime);

	bool startNetThread(unsigned short port, sf::Uint8 maxplayers);
	void stopNetThread();

	private:
	void netThread();
	void acceptNewConnections(std::list<sf::TcpSocket *> &newclients, sf::SocketSelector &selector);
	bool receivePlayerConnectionInfo(sf::TcpSocket *socket, sf::SocketSelector &selector); //Returns false if no information was received (true if some data was received or if the client disconnected).
	int receiveNewPackets(sf::Uint8 id, SafeSocket &socket);//Returns -1 if no error, and a DisconnectionReason otherwise

	bool playerNameExists(const std::string &name) const;

	std::thread *m_thread;
	bool m_thrrunning;

	IDCreator<sf::Uint8> m_playersids;
	std::mutex m_pidsmutex;
	sf::Uint8 m_maxplayers;//Not locked
	sf::Uint8 m_playerscount;

	std::mutex m_playerslistmutex;//Mutex locking the players list. Don't write to the players list from the child thread.

	sf::TcpListener m_listener;//Not locked
	std::unordered_map<sf::Uint8, SafeSocket> m_clients;//Don't write to the container in main thread.
	std::mutex m_clientmutex;

	std::list<std::pair<sf::TcpSocket *, Player>> m_acceptedplayers;//Write : child. Read : main.
	std::mutex m_acceptmutex;
	std::list<std::pair<sf::Uint8, sf::Packet>> m_receivedpackets;//Write : child. Read : main.
	std::mutex m_receivemutex;
	std::list<std::pair<sf::Uint8, sf::Uint8>> m_disconnectedplayers;//Write : child. Read : main.
	std::mutex m_disconnectmutex;

	std::list<sf::Uint8> m_clientstoremove;//Write : main. Read : child.
	std::mutex m_remclientmutex;
};

#endif // SERVERSIMULATOR_HPP_INCLUDED
