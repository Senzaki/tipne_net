#ifndef IDCREATOR_HPP_INCLUDED
#define IDCREATOR_HPP_INCLUDED

#include <list>

template<typename IDType>
class IDCreator
{
	public:
	IDCreator();

	IDType getNewID();//Throws std::length_error if full
	void reserveID(IDType id);
	void releaseID(IDType id);

	private:
	std::list<std::pair<IDType, IDType>> m_freeids;
	const IDType m_min, m_max;
};

#include "IDCreator.inl"

#endif // IDCREATOR_HPP_INCLUDED
