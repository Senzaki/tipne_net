#ifndef SAFELIST_HPP_INCLUDED
#define SAFELIST_HPP_INCLUDED

#include <list>
#include <functional>
#include <thread>
#include <mutex>

template<typename T>
class SafeList
{
	public:
	SafeList();
	SafeList(const SafeList &other);
	SafeList(SafeList &&other);

	SafeList &operator=(const SafeList &other);
	SafeList &operator=(SafeList &&other);

	template<typename... Args>
	void emplaceBack(Args &&...args);
	void pushBack(const T &arg);
	void pushBack(T &&arg);
	void popBack();
	void clear();
	bool empty();
	void foreach(std::function<void(T &)> tocall);

	private:
	std::list<T> m_list;
	std::mutex m_mutex;
};

#include "SafeList.inl"

#endif // SAFELIST_HPP_INCLUDED
