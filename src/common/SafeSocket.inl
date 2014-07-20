#include "make_unique.hpp"

template<typename SocketType>
SafeSocket<SocketType>::SafeSocket():
	m_socket(make_unique<SocketType>())
{

}

template<typename SocketType>
SafeSocket<SocketType>::SafeSocket(std::unique_ptr<SocketType> &&socket):
	m_socket(std::move(socket))
{

}

template<typename SocketType>
SafeSocket<SocketType>::SafeSocket(SafeSocket &&other):
	m_socket(std::move(other.m_socket))
{
	other.m_socket.reset();
}

template<typename SocketType>
void SafeSocket<SocketType>::addTo(sf::SocketSelector &selector) const
{
	selector.add(*m_socket);
}

template<typename SocketType>
void SafeSocket<SocketType>::removeFrom(sf::SocketSelector &selector) const
{
	selector.remove(*m_socket);
}

template<typename SocketType>
bool SafeSocket<SocketType>::isReady(sf::SocketSelector &selector) const
{
	return selector.isReady(*m_socket);
}

template<typename SocketType>
sf::Socket::Status SafeSocket<SocketType>::connect(const sf::IpAddress &addr, unsigned short port, sf::Time timeout)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_socket->connect(addr, port, timeout);
}

template<typename SocketType>
void SafeSocket<SocketType>::disconnect()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_socket->disconnect();
}

template<typename SocketType>
sf::Socket::Status SafeSocket<SocketType>::bind(unsigned short port)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_socket->bind(port);
}

template<typename SocketType>
void SafeSocket<SocketType>::unbind()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_socket->unbind();
}

template<typename SocketType>
unsigned short SafeSocket<SocketType>::getLocalPort()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_socket->getLocalPort();
}

template<typename SocketType>
template<typename... Args>
sf::Socket::Status SafeSocket<SocketType>::send(Args &&...args)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_socket->send(args...);
}

template<typename SocketType>
template<typename... Args>
sf::Socket::Status SafeSocket<SocketType>::receive(Args &&...args)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_socket->receive(args...);
}

template<typename SocketType>
void SafeSocket<SocketType>::setBlocking(bool blocking)
{
	m_socket->setBlocking(blocking);
}
