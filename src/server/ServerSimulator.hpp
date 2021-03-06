#ifndef SERVERSIMULATOR_HPP_INCLUDED
#define SERVERSIMULATOR_HPP_INCLUDED

#include "GameSimulator.hpp"
#include "IDCreator.hpp"
#include "ServerUdpManager.hpp"
#include "NetworkMessagesManager.hpp"

class ServerSimulator : public GameSimulator
{
	public:
	ServerSimulator(bool pure);
	virtual ~ServerSimulator();

	virtual bool update(float etime);
	void buildSnapshotPacket(sf::Packet &packet, sf::Uint8 playerid);

	bool startNetThread(unsigned short tcpport, unsigned short udpport, sf::Uint8 maxplayers);
	void stopNetThread();

	void disconnectPlayer(sf::Uint8 id, sf::Uint8 reason);

	virtual void selfCastSpell(const Spell &spell);

	protected:
	virtual void onNewRoundStarted(const std::string &mapname);
	virtual void onEntityAdded(GameEntity *entity);
	virtual void onEntityRemoved(GameEntity *entity);

	private:
	void netThread();
	void acceptNewConnections(std::list<std::unique_ptr<sf::TcpSocket>> &newclients, sf::SocketSelector &selector);
	bool receivePlayerConnectionInfo(std::unique_ptr<sf::TcpSocket> &socket, sf::SocketSelector &selector); //Returns false if no information was received (true if some data was received or if the client disconnected).
	int receiveNewPackets(sf::Uint8 id, SafeSocket<sf::TcpSocket> &socket);//Returns -1 if no error, and a DisconnectionReason otherwise

	void acceptNewPlayer(const sf::IpAddress &address, unsigned short port, Player &toaccept);
	void parseNewPacket(std::tuple<sf::Uint8, std::unique_ptr<sf::Packet>> &received);
	sf::Socket::Status sendToPlayer(sf::Uint8 id, sf::Packet &packet);

	bool onSetDirectionPacketReceived(sf::Uint8 sender, sf::Packet &packet);
	bool onCastSpellPacketReceived(sf::Uint8 sender, sf::Packet &packet);

	bool playerNameExists(const std::string &name) const;

	void resetRoundInfo();
	void updateVisibility();

	std::unique_ptr<std::thread> m_thread;
	std::atomic<bool> m_thrrunning;

	IDCreator<sf::Uint8> m_playersids;//Don't use it in main thread
	sf::Uint8 m_maxplayers;
	IDCreator<sf::Uint16> m_entitiesids;//Don't use it in child thread
	std::unordered_map<sf::Uint8, Character *> m_playerschars;

	sf::TcpListener m_listener;//Not locked
	std::unordered_map<sf::Uint8, SafeSocket<sf::TcpSocket>> m_clients;//Don't write to the container in main thread
	std::mutex m_clientsmutex;
	ServerUdpManager m_udpmgr;
	NetworkMessagesManager m_messages;

	//Communication between main thread/child thread
	//Child->Main
	SafeList<std::tuple<sf::IpAddress, unsigned short, Player>> m_acceptedplayers;
	SafeList<std::tuple<sf::Uint8, std::unique_ptr<sf::Packet>>> m_receivedpackets;
	SafeList<std::tuple<sf::Uint8, sf::Uint8>> m_disconnectedplayers;
	//Main->Child
	SafeList<sf::Uint8> m_clientstoremove;
};

#endif // SERVERSIMULATOR_HPP_INCLUDED
