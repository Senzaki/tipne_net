#ifndef SAFESOCKET_HPP_INCLUDED
#define SAFESOCKET_HPP_INCLUDED

#include <SFML/Network.hpp>
#include <mutex>

template<typename SocketType>
class SafeSocket
{
	public:
	SafeSocket();
	SafeSocket(SocketType *socket);//socket will be deleted within the destructor
	SafeSocket(SafeSocket<SocketType> &&other);
	~SafeSocket();

	SafeSocket(const SafeSocket<SocketType> &) = delete;
	SafeSocket &operator=(const SafeSocket<SocketType> &) = delete;

	void addTo(sf::SocketSelector &selector) const;
	void removeFrom(sf::SocketSelector &selector) const;
	bool isReady(sf::SocketSelector &selector) const;

	sf::Socket::Status connect(const sf::IpAddress &addr, unsigned short port, sf::Time timeout);//Only available for TCP sockets
	void disconnect();//Only available for TCP sockets
	sf::Socket::Status bind(unsigned short port);//Only available for UDP sockets
	void unbind();//Only available for UDP sockets
	unsigned short getLocalPort();

	template<typename... Args>
	sf::Socket::Status send(Args &&...args);
	template<typename... Args>
	sf::Socket::Status receive(Args &&...args);

	void setBlocking(bool blocking);//Not thread-safe (not really needed)

	private:
	SocketType *m_socket;
	std::mutex *m_mutex;
};

#include "SafeSocket.inl"

#endif // SAFESOCKET_HPP_INCLUDED
