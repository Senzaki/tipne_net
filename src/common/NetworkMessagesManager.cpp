#include "NetworkMessagesManager.hpp"
#include <iostream>
#include <cassert>

NetworkMessagesManager::NetworkMessagesManager():
	m_split(false)
{

}

void NetworkMessagesManager::addSocket(sf::Uint8 id, SafeSocket<sf::TcpSocket> &socket)
{
	//Force splitting to avoid sending data that were added before its connection to a receiver
	if(m_general.getDataSize() != 0)
		split();

	m_receivers.insert(std::make_pair(id, MessageReceiver(&socket)));
}

void NetworkMessagesManager::removeSocket(sf::Uint8 id)
{
	if(m_receivers.erase(id) == 0)
		std::cerr << "Cannot remove a socket of the network messages manager." << std::endl;
}

sf::Uint32 NetworkMessagesManager::getSequenceNumber(sf::Uint8 id) const
{
	return m_receivers.at(id).seqnbr;
}

void NetworkMessagesManager::append(const sf::Packet &packet)
{
	if(m_split)
	{
		for(std::pair<const sf::Uint8, MessageReceiver> &receiver : m_receivers)
		{
			MessageReceiver &info = receiver.second;
			info.packet.append(packet.getData(), packet.getDataSize());
		}
	}
	else
	{
		m_general.append(packet.getData(), packet.getDataSize());
	}
}

sf::Packet &NetworkMessagesManager::getIndividualPacket(sf::Uint8 id)
{
	//If we don't use split packets yet, copy the general packet into each individual packet
	if(!m_split)
		split();
	return m_receivers.at(id).packet;
}

void NetworkMessagesManager::sendMessages()
{
	//Send individual packets if split, m_general otherwise
	if(m_split)
	{
		for(std::pair<const sf::Uint8, MessageReceiver> &receiver : m_receivers)
		{
			MessageReceiver &info = receiver.second;
			if(info.packet.getDataSize() != 0)
			{
				info.socket->send(info.packet);
				info.seqnbr++;
				info.packet.clear();
			}
		}
		//Packets were cleared, go back to "do not split" state
		m_split = false;
	}
	else
	{
		if(m_general.getDataSize() != 0)
		{
			for(std::pair<const sf::Uint8, MessageReceiver> &receiver : m_receivers)
			{
				MessageReceiver &info = receiver.second;
				info.socket->send(m_general);
				info.seqnbr++;
			}
			m_general.clear();
		}
	}
}

void NetworkMessagesManager::split()
{
	m_split = true;
	for(std::pair<const sf::Uint8, MessageReceiver> &receiver : m_receivers)
		receiver.second.packet.append(m_general.getData(), m_general.getDataSize());
	m_general.clear();
}
