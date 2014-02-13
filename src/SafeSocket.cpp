#include "SafeSocket.hpp"

SafeSocket::SafeSocket():
	m_socket(new sf::TcpSocket()),
	m_mutex(new std::mutex)
{

}

SafeSocket::SafeSocket(sf::TcpSocket *socket):
	m_socket(socket),
	m_mutex(new std::mutex)
{

}

SafeSocket::SafeSocket(SafeSocket &&other):
	m_socket(other.m_socket),
	m_mutex(other.m_mutex)
{
	other.m_mutex = nullptr;
	other.m_socket = nullptr;
}

SafeSocket::~SafeSocket()
{
	delete m_mutex;
	delete m_socket;
}

void SafeSocket::addTo(sf::SocketSelector &selector) const
{
	selector.add(*m_socket);
}

void SafeSocket::removeFrom(sf::SocketSelector &selector) const
{
	selector.remove(*m_socket);
}

bool SafeSocket::isReady(sf::SocketSelector &selector) const
{
	return selector.isReady(*m_socket);
}

sf::Socket::Status SafeSocket::connect(const sf::IpAddress &addr, unsigned short port, sf::Time timeout)
{
	std::lock_guard<std::mutex> lock(*m_mutex);
	return m_socket->connect(addr, port, timeout);
}

sf::Socket::Status SafeSocket::send(sf::Packet &packet)
{
	std::lock_guard<std::mutex> lock(*m_mutex);
	return m_socket->send(packet);
}

sf::Socket::Status SafeSocket::receive(sf::Packet &packet)
{
	std::lock_guard<std::mutex> lock(*m_mutex);
	return m_socket->receive(packet);
}

void SafeSocket::disconnect()
{
	std::lock_guard<std::mutex> lock(*m_mutex);
	m_socket->disconnect();
}

void SafeSocket::setBlocking(bool blocking)
{
	m_socket->setBlocking(blocking);
}
