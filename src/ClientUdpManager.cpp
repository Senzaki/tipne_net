#include "ClientUdpManager.hpp"
#include "ClientSimulator.hpp"
#include "make_unique.hpp"
#include <iostream>
#include <cassert>

static const sf::Time SELECTOR_WAIT_TIME = sf::seconds(0.2f);
static const float UDP_TIMEOUT = 10.f;
static const float CONNECTION_INTERVAL = 0.5f;
static const float KEEPALIVE_INTERVAL = 2.f;

ClientUdpManager::ClientUdpManager(ClientSimulator &simulator):
	m_thread(nullptr),
	m_thrrunning(false),
	m_simulator(simulator)
{

}

ClientUdpManager::~ClientUdpManager()
{
	stopNetThread();
}

bool ClientUdpManager::update(float etime)
{
	if(!m_thrrunning)
		return false;

	bool success = true;

	//Send a keep-alive if required
	m_lastpacketsent += etime;
	if(m_lastpacketsent > m_keepaliveinterval)
	{
		m_lastpacketsent = 0.f;
		sf::Packet keepalive;
		keepalive << (sf::Uint8)UdpPacketType::KeepAlive;
		sendPacket(keepalive);
	}

	if(m_receivedpackets.empty())
	{
		//Connection timeout ?
		m_lastpacketreceived += etime;
		if(m_lastpacketreceived > UDP_TIMEOUT)
		{
			std::cerr << "UDP connection timeout." << std::endl;
			return false;
		}
	}
	else
	{
		m_lastpacketreceived = 0.f;
		m_keepaliveinterval = KEEPALIVE_INTERVAL;//Stop connection attempts
		//Treat the packets
		m_receivedpackets.treat([this, &success](std::unique_ptr<sf::Packet> &packet)
		{
			if(success)
				success = parseReceivedPacket(*packet);
		});
	}

	return success;
}

unsigned short ClientUdpManager::startNetThread(sf::IpAddress addr, unsigned short udpport)
{
	assert(!m_thrrunning);
	m_address = addr;
	m_port = udpport;
	//Bind the socket to a random free port
	if(m_server.bind(sf::Socket::AnyPort) != sf::Socket::Done)
		return 0;
	m_server.setBlocking(false);
	//Start the UDP networking thread
	m_thrrunning = true;
	try
	{
		m_thread = make_unique<std::thread>(&ClientUdpManager::netThread, this);
	}
	catch(const std::exception &e)
	{
		//Thread could not start
		m_thrrunning = false;
		m_thread.reset();
		std::cerr << "Cannot start UDP networking thread." << std::endl;
		std::cerr << e.what() << std::endl;
		m_server.unbind();
		return 0;
	}
	m_lastpacketreceived = 0.f;
	m_lastpacketsent = CONNECTION_INTERVAL;
	m_keepaliveinterval = CONNECTION_INTERVAL;
	return m_server.getLocalPort();
}

void ClientUdpManager::stopNetThread()
{
	if(m_thread)
	{
		//Ask the thread to stop
		m_thrrunning = false;
		//Wait for it to finish
		try
		{
			m_thread->join();
		}
		catch(const std::system_error &e)
		{
			std::cerr << "Could not stop UDP networking thread." << std::endl;
			std::cerr << e.what() << std::endl;
		}
		m_thread.reset();
		//Delete the remaining packets
		m_receivedpackets.clear();
	}
}

void ClientUdpManager::netThread()
{
	//Add the socket to a selector
	sf::SocketSelector selector;
	m_server.addTo(selector);

	while(m_thrrunning)
	{
		//Wait for data
		if(selector.wait(SELECTOR_WAIT_TIME))
		{
			//Data was received, retrieve it
			if(!receiveNewPackets())
			{
				std::cerr << "Error while receiving data. Disconnecting." << std::endl;
				m_thrrunning = false;
			}
		}
	}

	m_server.unbind();
}

bool ClientUdpManager::receiveNewPackets()
{
	sf::Socket::Status status;
	auto packet = make_unique<sf::Packet>();
	sf::IpAddress senderaddr;
	unsigned short senderport;
	//Receive all the packets
	while((status = m_server.receive(*packet, senderaddr, senderport)) == sf::Socket::Done)
	{
		//Was it really the server that sent it ?
		if(senderaddr != m_address && senderport != m_port)
		{
			std::cerr << "[Warning]UDP packet received, but not from server. Ignoring it." << std::endl;
			continue;
		}
		m_receivedpackets.emplaceBack(std::move(packet));
		packet =make_unique<sf::Packet>();
	}
	//Error, or just no packets left ?
	if(status == sf::Socket::NotReady)
		return true;
	else if(status == sf::Socket::Error)
		std::cerr << "Unexpected network error." << std::endl;
	return false;
}

void ClientUdpManager::sendPacket(sf::Packet &packet)
{
	m_lastpacketsent = 0.f;
	m_server.send(packet, m_address, m_port);
}

bool ClientUdpManager::parseReceivedPacket(sf::Packet &packet)
{
	sf::Uint8 type;
	if(!(packet >> type))
		return false;
	switch(type)
	{
		case (sf::Uint8)UdpPacketType::KeepAlive:
			break;

		case (sf::Uint8)UdpPacketType::Snapshot:
			if(!m_simulator.onSnapshotReceived(packet))
				return false;
			break;

		default:
			return false;
	}

	return true;
}
