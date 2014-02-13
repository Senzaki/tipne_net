#ifndef SAFESOCKET_HPP_INCLUDED
#define SAFESOCKET_HPP_INCLUDED

#include <SFML/Network.hpp>
#include <mutex>

class SafeSocket
{
	public:
	SafeSocket();
	SafeSocket(sf::TcpSocket *socket);//socket will be deleted within the destructor
	SafeSocket(SafeSocket &&other);
	~SafeSocket();

	SafeSocket(const SafeSocket &) = delete;
	SafeSocket &operator=(const SafeSocket &) = delete;

	void addTo(sf::SocketSelector &selector) const;
	void removeFrom(sf::SocketSelector &selector) const;
	bool isReady(sf::SocketSelector &selector) const;

	sf::Socket::Status connect(const sf::IpAddress &addr, unsigned short port, sf::Time timeout = sf::Time::Zero);
	sf::Socket::Status send(sf::Packet &packet);
	sf::Socket::Status receive(sf::Packet &packet);
	void disconnect();

	void setBlocking(bool blocking);//Not thread-safe (not really needed)

	private:
	sf::TcpSocket *m_socket;
	std::mutex *m_mutex;
};

#endif // SAFESOCKET_HPP_INCLUDED
