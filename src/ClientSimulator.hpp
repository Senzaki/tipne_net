#ifndef CLIENTSIMULATOR_HPP_INCLUDED
#define CLIENTSIMULATOR_HPP_INCLUDED

#include "GameSimulator.hpp"
#include <SFML/Network.hpp>
#include <thread>
#include <mutex>

class ClientSimulator : public GameSimulator
{
	public:
	ClientSimulator();
	virtual ~ClientSimulator();

	int startNetThread(const sf::IpAddress &serveraddr, unsigned short port, const std::string &name); //Returns -1 in case of error, and the connection status otherwise
	void stopNetThread();

	private:
	bool parseConnectionData(sf::Packet &packet);

	void netThread();

	std::thread *m_thread;
	std::mutex m_mutex;
	bool m_thrrunning;

	sf::TcpSocket m_server;

	sf::Uint8 m_id;
};

#endif // CLIENTSIMULATOR_HPP_INCLUDED
