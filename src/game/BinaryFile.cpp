#include "BinaryFile.hpp"
#include <SFML/System.hpp>

//Include headers to convert endianness
#if defined(SFML_SYSTEM_WINDOWS)
#include <winsock2.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#endif

BinaryFile::BinaryFile()
{

}

BinaryFile::BinaryFile(const char *path, std::ios::openmode mode):
	m_file(path, std::ios::binary | mode)
{

}

void BinaryFile::open(const char *path, std::ios::openmode mode)
{
	m_file.open(path, std::ios::binary | mode);
}

void BinaryFile::close()
{
	m_file.close();
}

bool BinaryFile::operator!() const
{
	return !m_file;
}

BinaryFile::operator bool() const
{
	return m_file;
}

BinaryFile &BinaryFile::operator<<(sf::Int8 n)
{
	m_file.put(n);
	return *this;
}

BinaryFile &BinaryFile::operator<<(sf::Uint8 n)
{
	m_file.put(n);
	return *this;
}

BinaryFile &BinaryFile::operator<<(sf::Int16 n)
{
	sf::Int16 e = htons(n);
	m_file.write(reinterpret_cast<const char *>(&e), sizeof(e));
	return *this;
}

BinaryFile &BinaryFile::operator<<(sf::Uint16 n)
{
	sf::Uint16 e = htons(n);
	m_file.write(reinterpret_cast<const char *>(&e), sizeof(e));
	return *this;
}

BinaryFile &BinaryFile::operator<<(sf::Int32 n)
{
	sf::Int32 e = htonl(n);
	m_file.write(reinterpret_cast<const char *>(&e), sizeof(e));
	return *this;
}

BinaryFile &BinaryFile::operator<<(sf::Uint32 n)
{
	sf::Uint32 e = htonl(n);
	m_file.write(reinterpret_cast<const char *>(&e), sizeof(e));
	return *this;
}

BinaryFile &BinaryFile::operator<<(const char *str)
{
	m_file << str;
	return *this;
}

BinaryFile &BinaryFile::operator>>(sf::Int8 &n)
{
	n = m_file.get();
	return *this;
}

BinaryFile &BinaryFile::operator>>(sf::Uint8 &n)
{
	n = m_file.get();
	return *this;
}

BinaryFile &BinaryFile::operator>>(sf::Int16 &n)
{
	m_file.read(reinterpret_cast<char *>(&n), sizeof(n));
	n = ntohs(n);
	return *this;
}

BinaryFile &BinaryFile::operator>>(sf::Uint16 &n)
{
	m_file.read(reinterpret_cast<char *>(&n), sizeof(n));
	n = ntohs(n);
	return *this;
}

BinaryFile &BinaryFile::operator>>(sf::Int32 &n)
{
	m_file.read(reinterpret_cast<char *>(&n), sizeof(n));
	n = ntohl(n);
	return *this;
}

BinaryFile &BinaryFile::operator>>(sf::Uint32 &n)
{
	m_file.read(reinterpret_cast<char *>(&n), sizeof(n));
	n = ntohl(n);
	return *this;
}

BinaryFile &BinaryFile::operator>>(std::string &str)
{
	m_file >> str;
	return *this;
}
