#ifndef SERVERSIMULATOR_HPP_INCLUDED
#define SERVERSIMULATOR_HPP_INCLUDED

#include <SFML/Network.hpp>

class ServerSimulator
{
	public:
	ServerSimulator();

	bool startNetThread(const sf::IpAddress &listenaddr, unsigned short port);
	void stopNetThread();

	private:
	void netThread();

	sf::Thread m_thread;
	sf::Mutex m_mutex;
	bool m_thrrunning;
};

#endif // SERVERSIMULATOR_HPP_INCLUDED
