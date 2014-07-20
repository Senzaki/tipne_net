#include <type_traits>

template<typename T>
NetworkMessagesManager &NetworkMessagesManager::operator<<(T &&data)
{
	typedef typename std::remove_reference<T>::type NoRefType;
	static_assert(!std::is_same<NoRefType, sf::Packet>::value, "Cannot call operator<< with object of type sf::Packet. Use append() instead.");
	//Do we use split packets ? (Yes : Append the data to each individual packet. No : Append the data to the general packet.)
	if(m_split)
	{
		for(std::pair<const sf::Uint8, MessageReceiver> &receiver : m_receivers)
			receiver.second.packet << std::forward<T>(data);
	}
	else
		m_general << std::forward<T>(data);
	return *this;
}
