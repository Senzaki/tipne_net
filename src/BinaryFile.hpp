#ifndef BINARYFILE_HPP_INCLUDED
#define BINARYFILE_HPP_INCLUDED

#include <fstream>
#include <SFML/System.hpp>

class BinaryFile
{
	public:
	BinaryFile();
	BinaryFile(const char *path, std::ios::openmode mode);

	void open(const char *path, std::ios::openmode mode);
	void close();

	bool operator!() const;
	operator bool() const;

	BinaryFile &operator<<(sf::Int8 n);
	BinaryFile &operator<<(sf::Uint8 n);
	BinaryFile &operator<<(sf::Int16 n);
	BinaryFile &operator<<(sf::Uint16 n);
	BinaryFile &operator<<(sf::Int32 n);
	BinaryFile &operator<<(sf::Uint32 n);
	BinaryFile &operator<<(float n);
	BinaryFile &operator<<(const char *str);

	BinaryFile &operator>>(sf::Int8 &n);
	BinaryFile &operator>>(sf::Uint8 &n);
	BinaryFile &operator>>(sf::Int16 &n);
	BinaryFile &operator>>(sf::Uint16 &n);
	BinaryFile &operator>>(sf::Int32 &n);
	BinaryFile &operator>>(sf::Uint32 &n);
	BinaryFile &operator>>(float &n);
	BinaryFile &operator>>(std::string &str);

	private:
	std::fstream m_file;
};

#endif // BINARYFILE_HPP_INCLUDED
