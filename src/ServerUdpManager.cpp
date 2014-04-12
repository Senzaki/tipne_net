#include "ServerUdpManager.hpp"
#include "ServerSimulator.hpp"
#include <iostream>
#include <cassert>

static const sf::Time SELECTOR_WAIT_TIME = sf::seconds(0.2f);
static const float UDP_TIMEOUT = 7.f;

template<int hashsize>
size_t ServerUdpManager::hashPlayerInfo(const PlayerInfo &info)
{
	return static_cast<size_t>(info.address.toInteger()) | (static_cast<size_t>(info.port) << 32);
}

//Specialization for little size_t types (we consider that it will always be at least 4 bytes long)
template<>
size_t ServerUdpManager::hashPlayerInfo<4>(const PlayerInfo &info)
{
	return info.address.toInteger() ^ (static_cast<size_t>(info.port) << (32 - sizeof(info.port) * 8));
}

bool ServerUdpManager::PlayerInfo::operator==(const PlayerInfo &other) const
{
	return address == other.address && port == other.port;
}

ServerUdpManager::ServerUdpManager(ServerSimulator &simulator):
	m_thread(nullptr),
	m_thrrunning(false),
	m_simulator(simulator),
	m_playersinfo(8, &ServerUdpManager::hashPlayerInfo<sizeof(size_t)>)
{

}

ServerUdpManager::~ServerUdpManager()
{
	stopNetThread();
}

void ServerUdpManager::update(float etime)
{
	assert(m_thrrunning);

	m_lastsnapshot += etime;
	if(m_lastsnapshot >= DEFAULT_SNAPSHOT_TIME)
	{
		m_lastsnapshot = 0.f;
		sf::Packet packet;
		m_simulator.buildSnapshotPacket(packet);
		sendToAll(packet);
	}

	//Treat the packets
	m_receivedpackets.foreach([this](std::tuple<sf::IpAddress, unsigned short, sf::Packet *> &packet)
	{
		sf::Uint8 id = getAssociatedPlayer(std::get<0>(packet), std::get<1>(packet));
		if(id != NEUTRAL_PLAYER)
			parseReceivedPacket(*std::get<2>(packet), id);
		delete std::get<2>(packet);
	});

	auto it = m_lastpacketreceived.begin();
	while(it != m_lastpacketreceived.end())
	{
		it->second += etime;
		if(it->second >= UDP_TIMEOUT)
		{
			std::cerr << "UDP timeout. Disconnecting client." << std::endl;
			sf::Uint8 id = it->first;
			it++;
			m_simulator.disconnectPlayer(id, (sf::Uint8)DisconnectionReason::Timeout);
		}
		else
			it++;
	}

	m_receivedpackets.clear();
}

bool ServerUdpManager::startNetThread(unsigned short udpport)
{
	//If the thread already exists, don't start a new one
	if(m_thread)
		return false;

	//Prepare the UDP socket
	if(m_socket.bind(udpport) != sf::Socket::Done)
	{
		std::cerr << "Cannot bind UDP socket to port " << udpport << "." << std::endl;
		return false;
	}
	m_socket.setBlocking(false);

	//Create the thread
	m_thrrunning = true;
	try
	{
		m_thread = new std::thread(&ServerUdpManager::netThread, this);
	}
	catch(const std::exception &e)
	{
		m_thrrunning = false;
		std::cerr << "Could not start UDP networking thread." << std::endl;
		std::cerr << e.what() << std::endl;
		m_thread = nullptr;
		return false;
	}
	m_lastsnapshot = 0.f;
	return true;
}

void ServerUdpManager::stopNetThread()
{
	if(m_thread)
	{
		//Stop the child thread
		m_thrrunning = false;
		//Wait for the thread to stop
		try
		{
			m_thread->join();
		}
		catch(const std::system_error &e)
		{
			std::cerr << "Could not stop UDP networking thread." << std::endl;
			std::cerr << e.what() << std::endl;
		}
		delete m_thread;
		m_thread = nullptr;

		//Delete received packets
		m_receivedpackets.foreach([](std::tuple<sf::IpAddress, unsigned short, sf::Packet *> &received)
		{
			delete std::get<2>(received);
		});
		m_receivedpackets.clear();
	}
}

bool ServerUdpManager::addPlayer(sf::Uint8 id, sf::IpAddress addr, unsigned short port)
{
	PlayerInfo pinfo;
	pinfo.address = addr;
	pinfo.port = port;
	//Ensure this player/address/port set has not already been added
	if(!m_lastpacketreceived.emplace(id, 0.f).second)
		return false;
	if(!m_playersinfo.emplace(pinfo, id).second)
	{
		m_lastpacketreceived.erase(id);
		return false;
	}
	return true;
}

void ServerUdpManager::removePlayer(sf::Uint8 id)
{
	assert(m_lastpacketreceived.count(id) != 0);
	m_lastpacketreceived.erase(id);
	//Also find the address/port pair associated to this id and remove it
	for(auto it = m_playersinfo.begin(); it != m_playersinfo.end(); it++)
	{
		if(it->second == id)
		{
			m_playersinfo.erase(it);
			return;
		}
	}
#ifndef NDEBUG
	//A little assert without assert, we shouldn't reach the end of this func ^^
	std::cerr << "In " << __FILE__ << " at line " << __LINE__ << " : No player info corresponding to this id was found." << std::endl;
	abort();
#endif
}

void ServerUdpManager::netThread()
{
	sf::SocketSelector selector;
	m_socket.addTo(selector);

	while(m_thrrunning)
	{
		if(selector.wait(SELECTOR_WAIT_TIME))
			receiveNewPackets();
	}

	m_socket.unbind();
}

void ServerUdpManager::receiveNewPackets()
{
	sf::Socket::Status status;
	sf::Packet *packet = new sf::Packet;
	sf::IpAddress senderaddr;
	unsigned short senderport;
	//Receive all the packets
	while((status = m_socket.receive(*packet, senderaddr, senderport)) == sf::Socket::Done)
	{
		m_receivedpackets.emplaceBack(senderaddr, senderport, packet);
		packet = new sf::Packet;
	}
	//Delete the last (unused) packet
	delete packet;
	//Error, or just no packets left ?
	if(status == sf::Socket::Error)
		std::cerr << "Unexpected UDP network error." << std::endl;
}

void ServerUdpManager::sendToAll(sf::Packet &packet)
{
	for(const std::pair<const PlayerInfo, sf::Uint8> &player : m_playersinfo)
		m_socket.send(packet, player.first.address, player.first.port);
}

sf::Uint8 ServerUdpManager::getAssociatedPlayer(const sf::IpAddress &address, unsigned short port)
{
	try
	{
		return m_playersinfo.at(PlayerInfo{address, port});
	}
	catch(const std::out_of_range &)
	{

	}
	return NEUTRAL_PLAYER;
}

void ServerUdpManager::parseReceivedPacket(sf::Packet &packet, sf::Uint8 id)
{
	if(!m_simulator.playerExists(id))
	{
#ifndef NDEBUG
		std::cerr << "[DEBUG]UDP packet received from unknown client. Discarding it." << std::endl;
#endif
		return;
	}

	//Change time since last packet
	try
	{
		m_lastpacketreceived.at(id) = 0.f;
	}
	catch(const std::out_of_range &)
	{

	}

	//Which type of packet ?
	sf::Uint8 type;
	if(!(packet >> type))
		return;
	switch(type)
	{
		case (sf::Uint8)UdpPacketType::KeepAlive:
			break;//Just a keep-alive, ignore

		default:
			std::cerr << "Invalid packet received from " << (int)id << '.' << std::endl;
			break;
	}
}
