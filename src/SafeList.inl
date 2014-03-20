template<typename T>
SafeList<T>::SafeList()
{

}

template<typename T>
SafeList<T>::SafeList(const SafeList &other)
{
	std::lock_guard<std::mutex> olock(other.m_mutex);
	m_list = other.m_list;
}

template<typename T>
SafeList<T>::SafeList(SafeList &&other):
	m_list(std::move(other.m_list))
{

}

template<typename T>
SafeList<T> &SafeList<T>::operator=(const SafeList &other)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	std::lock_guard<std::mutex> olock(other.m_mutex);
	m_list = other.m_list;
}

template<typename T>
SafeList<T> &SafeList<T>::operator=(SafeList &&other)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_list = std::move(other.m_list);
}

template<typename T>
template<typename... Args>
void SafeList<T>::emplaceBack(Args &&...args)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_list.emplace_back(std::forward<Args>(args)...);
}

template<typename T>
void SafeList<T>::pushBack(T &&arg)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_list.push_back(std::forward<T>(arg));
}

template<typename T>
void SafeList<T>::pushBack(const T &arg)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_list.push_back(arg);
}

template<typename T>
void SafeList<T>::popBack()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_list.pop_back();
}

template<typename T>
void SafeList<T>::clear()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_list.clear();
}

template<typename T>
void SafeList<T>::foreach(std::function<void(T &)> tocall)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	for(T &item : m_list)
		tocall(item);
}