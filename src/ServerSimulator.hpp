#ifndef SERVERSIMULATOR_HPP_INCLUDED
#define SERVERSIMULATOR_HPP_INCLUDED

#include "GameSimulator.hpp"
#include "IDCreator.hpp"
#include "SafeSocket.hpp"
#include <thread>
#include <atomic>
#include "SafeList.hpp"
#include <tuple>

class ServerSimulator : public GameSimulator
{
	public:
	ServerSimulator(bool pure);
	virtual ~ServerSimulator();

	virtual bool update(float etime);

	virtual bool loadMap(sf::Uint8 mapid);
	bool startNetThread(unsigned short port, sf::Uint8 maxplayers);
	void stopNetThread();

	private:
	void netThread();
	void acceptNewConnections(std::list<sf::TcpSocket *> &newclients, sf::SocketSelector &selector);
	bool receivePlayerConnectionInfo(sf::TcpSocket *socket, sf::SocketSelector &selector); //Returns false if no information was received (true if some data was received or if the client disconnected).
	int receiveNewPackets(sf::Uint8 id, SafeSocket &socket);//Returns -1 if no error, and a DisconnectionReason otherwise

	void acceptNewPlayer(Player &toaccept);
	void parseNewPacket(std::tuple<sf::Uint8, sf::Packet *> &received);
	void disconnectPlayer(sf::Uint8 id, sf::Uint8 reason);
	sf::Socket::Status sendToPlayer(sf::Uint8 id, sf::Packet &packet);
	void sendToAllPlayers(sf::Packet &packet);

	virtual bool removeCharacter(sf::Uint16 id);

	bool playerNameExists(const std::string &name) const;

	std::thread *m_thread;
	std::atomic<bool> m_thrrunning;

	IDCreator<sf::Uint8> m_playersids;//Don't use it in main thread
	sf::Uint8 m_maxplayers;
	IDCreator<sf::Uint16> m_charactersids;//Don't use it in child thread

	sf::TcpListener m_listener;//Not locked
	std::unordered_map<sf::Uint8, SafeSocket> m_clients;//Don't write to the container in main thread
	std::mutex m_clientsmutex;

	//Communication between main thread/child thread
	//Child->Main
	SafeList<Player> m_acceptedplayers;
	SafeList<std::tuple<sf::Uint8, sf::Packet *>> m_receivedpackets;
	SafeList<std::tuple<sf::Uint8, sf::Uint8>> m_disconnectedplayers;
	//Main->Child
	SafeList<sf::Uint8> m_clientstoremove;
};

#endif // SERVERSIMULATOR_HPP_INCLUDED
