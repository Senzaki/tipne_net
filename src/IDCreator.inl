#include <stdexcept>
#include <limits>

template <typename IDType>
IDCreator<IDType>::IDCreator():
	m_min(std::numeric_limits<IDType>::min()),
	m_max(std::numeric_limits<IDType>::max())
{
	m_freeids.emplace_back(m_min, m_max);
}

template<typename IDType>
IDType IDCreator<IDType>::getNewID()
{
	if(m_freeids.empty())
		throw std::length_error("No free ID left.");
	//Get the first free ID range
	auto &range = m_freeids.front();
	//Return the first available ID
	IDType id = range.first;
	//Reduce the size of the range (and remove it if it is empty)
	if(range.first == range.second)
		m_freeids.pop_front();
	else
		range.first++;
	return id;
}

template<typename IDType>
void IDCreator<IDType>::reserveID(IDType id)
{
	auto it = m_freeids.begin();
	//Find the corresponding range
	while(id > it->second)
	{
		it++;
		if(it == m_freeids.end())//ID already reserved
			return;
	}
	if(id < it->first)//ID already reserved
		return;
	//Remove the reserved id from the range
	if(id == it->first)
	{
		if(it->first == it->second)
			m_freeids.erase(it);
		else
			it->first++;
	}
	else if(id == it->second)
	{
		//The new range can't be empty, because id would be equal to it->first
		it->second--;
	}
	else
	{
		//New range : [first id; id - 1]
		m_freeids.emplace(it, it->first, id - 1);
		//Old range becomes [id + 1; last id]
		it->first = id + 1;
	}
}

template <typename IDType>
void IDCreator<IDType>::releaseID(IDType id)
{
	if(m_freeids.empty())
		m_freeids.emplace_back(id, id);
	else
	{
		//Find the corresponding range
		auto it = m_freeids.begin();
		while(it->second + 1 < id)
		{
			it++;
			if(it == m_freeids.end())
			{
				if(!m_freeids.empty())
				{
					if(m_freeids.back().second < id)
						m_freeids.emplace_back(id, id);
				}
				return;
			}
		}
		//Add to the beginning ?
		if(id + 1 == it->first)
			it->first--;
		else if(id < it->first) //Add a new range ?
			m_freeids.emplace(it, id, id);
		else if(id == it->second + 1) //Add to the end ?
		{
			it->second++;
			it++;
			//Merge two ranges if required
			if(it != m_freeids.end())
			{
				if(it->first == id + 1)
				{
					it->first = std::prev(it)->first;
					m_freeids.erase(std::prev(it));
				}
			}
		}
	}
}
