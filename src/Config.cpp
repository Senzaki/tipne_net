#include "Config.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <stdexcept>

static constexpr const char *FILENAME = "data/conf";

Config &Config::getInstance()
{
	static Config conf;
	return conf;
}

Config::Config():
	name("New Player"),
	width(sf::VideoMode::getDesktopMode().width),
	height(sf::VideoMode::getDesktopMode().height),
	fullscreen(true),
	vsync(true),
	dispfreq(0),
	lang(DEFAULT_LANGUAGE),
	connectto_ip("127.0.0.1"),
	connectto_port(DEFAULT_PORT),
	server_port(DEFAULT_PORT),
	max_players(16)
{
	using namespace std::placeholders;
	//Setup parsing functions table (1 command = 1 function)
	m_parsers["name"] = std::bind(&Config::parseString, this, _1, _2, std::ref(name));
	m_parsers["videomode"] = std::bind(&Config::parseVideoMode, this, _1, _2);
	m_parsers["fullscreen"] = std::bind(&Config::parseBool, this, _1, _2, std::ref(fullscreen));
	m_parsers["vsync"] = std::bind(&Config::parseBool, this, _1, _2, std::ref(vsync));
	m_parsers["dispfreq"] = std::bind(&Config::parseDispFreq, this, _1, _2);
	m_parsers["lang"] = std::bind(&Config::parseString, this, _1, _2, std::ref(lang));
	m_parsers["connectto_ip"] = std::bind(&Config::parseIpAddress, this, _1, _2, std::ref(connectto_ip));
	m_parsers["connectto_port"] = std::bind(&Config::parsePort, this, _1, _2, std::ref(connectto_port));
	m_parsers["server_port"] = std::bind(&Config::parsePort, this, _1, _2, std::ref(server_port));
	m_parsers["max_players"] = std::bind(&Config::parseMaxPlayers, this, _1, _2);

	load();
}

bool Config::load()
{
	std::ifstream file(FILENAME);

	//If the config file couldn't be opened
	if(!file)
	{
		std::cout << "Configuration file does not exist. Now creating it." << std::endl;

		//Try to create a default file
		if(!save())
		{
			std::cerr << "The default configuration file could not be created." << std::endl;
			return false;
		}
	}
	else
	{
		//Load the config file if the file could be opened.
		std::string line;
		std::string command;
		const char *args;
		size_t firstspace;

		//Parse each line
		while(std::getline(file, line))
		{
			if(line[0] == '#' || line.empty())
				continue;

			//Find the end of the command
			firstspace = line.find(' ');
			//Separate command and args
			command = (firstspace == std::string::npos) ? line : line.substr(0, firstspace);
			args = (firstspace == std::string::npos) ? "" : line.c_str() + firstspace + 1;
			try
			{
				//Call the appropriate parsing func
				m_parsers.at(command)(command.c_str(), args);
			}
			catch(const std::out_of_range &)
			{
				//command isn't a key of the map
				std::cerr << "Error : configuration option " << command << " does not exist." << std::endl;
			}
		}

		file.close();
	}

	return true;
}

bool Config::save() const
{
	std::ofstream file(FILENAME);

	//Just exit if the file couldn't be opened.
	if(!file)
	{
		std::cout << "Configuration file could not be created or opened for writing." << std::endl;
		return false;
	}

	//Write the configuration
	file << "name " << name
		 << "\nvideomode " << (int)width << " " << (int)height
		 << "\nfullscreen " << (int)fullscreen
		 << "\nvsync " << (int)vsync
		 << "\ndispfreq " << (int)dispfreq
		 << "\nlang " << lang
		 << "\nconnectto_ip " << connectto_ip
		 << "\nconnectto_port " << (int)connectto_port
		 << "\nserver_port " << (int)server_port
		 << "\nmax_players " << (int)max_players;

	file.close();

	return !file.fail();
}

void Config::writeError(const char *command)
{
	std::cerr << "Error while parsing configuration file : invalid value for \"" << command << "\" parameter." << std::endl;
}

bool Config::parseVideoMode(const char *paramname, const char *args)
{
	unsigned int tempw, temph;
	std::istringstream strm(args);//Parse args : wwww hhhh
	strm >> tempw >> temph;
	if(!strm || tempw < 800 || temph < 600)
	{
		writeError(paramname);
		return false;
	}

	width = tempw;
	height = temph;
	return true;
}

bool Config::parseBool(const char *paramname, const char *args, bool &param)
{
	int temp;
	std::istringstream strm(args);
	strm >> temp;
	if(!strm)
	{
		writeError(paramname);
		return false;
	}

	param = temp;
	return true;
}

bool Config::parseDispFreq(const char *paramname, const char *args)
{
	unsigned int temp;
	std::istringstream strm(args);
	strm >> temp;
	if(!strm)
	{
		writeError(paramname);
		return false;
	}

	dispfreq = temp;
	return true;
}

bool Config::parseString(const char *paramname, const char *args, std::string &param)
{
	//Check that the string is correct (not empty)
	if(args[0] == '\0')
	{
		writeError(paramname);
		return false;
	}

	param = std::move(args);
	return true;
}

bool Config::parseIpAddress(const char *paramname, const char *args, std::string &param)
{
	std::string newval;
	if(!parseString(paramname, args, newval))
		return false;
	//Check that this is a valid address
	sf::IpAddress addr(newval);
	if(addr == sf::IpAddress::None)
	{
		writeError(paramname);
		return false;
	}
	param = newval;
	return true;
}

bool Config::parsePort(const char *paramname, const char *args, unsigned short &param)
{
	unsigned int temp;
	std::istringstream strm(args);
	strm >> temp;
	if(!strm || temp > 65535)
	{
		writeError(paramname);
		return false;
	}

	param = temp;
	return true;
}

bool Config::parseMaxPlayers(const char *paramname, const char *args)
{
	unsigned int temp;
	std::istringstream strm(args);
	strm >> temp;
	if(!strm || temp > 254 || temp < 1)
	{
		writeError(paramname);
		return false;
	}

	max_players = temp;
	return true;
}
