#ifndef CONFIG_HPP_INCLUDED
#define CONFIG_HPP_INCLUDED

#include <string>
#include <functional>
#include <unordered_map>
#include <SFML/System.hpp>

constexpr const char *DEFAULT_LANGUAGE = "en";
const unsigned short DEFAULT_TCP_PORT = 57575;
const unsigned short DEFAULT_UDP_PORT = 57577;

struct Config
{
	static Config &getInstance();

	Config(const Config &) = delete;
	Config &operator=(const Config &) = delete;

	bool load();
	bool save() const;

	std::string name;
	unsigned int width;
	unsigned int height;
	bool fullscreen;
	bool vsync;
	unsigned int dispfreq;
	std::string lang;
	std::string connectto_ip;
	unsigned short connectto_tcpport;
	unsigned short connectto_udpport;
	unsigned short server_tcpport;
	unsigned short server_udpport;
	sf::Uint8 max_players;

	private:
	Config();

	std::unordered_map<std::string, std::function<bool(const char *, const char *)>> m_parsers;

	void writeError(const char *command);

	bool parseVideoMode(const char *paramname, const char *args);
	bool parseBool(const char *paramname, const char *args, bool &param);
	bool parseDispFreq(const char *paramname, const char *args);
	bool parseString(const char *paramname, const char *args, std::string &param);
	bool parseIpAddress(const char *paramname, const char *args, std::string &param);
	bool parsePort(const char *paramname, const char *args, unsigned short &param);
	bool parseMaxPlayers(const char *paramname, const char *args);
};

#endif // CONFIG_HPP_INCLUDED
