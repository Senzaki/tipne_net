#include "Config.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <SFML/Window.hpp>

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
	dispfreq(0)
{
	using namespace std::placeholders;
	//Setup parsing functions table (1 command = 1 function)
	m_parsers["name"] = std::bind(&Config::parseName, this, _1);
	m_parsers["videomode"] = std::bind(&Config::parseVideoMode, this, _1);
	m_parsers["fullscreen"] = std::bind(&Config::parseFullscreen, this, _1);
	m_parsers["vsync"] = std::bind(&Config::parseVSync, this, _1);
	m_parsers["dispfreq"] = std::bind(&Config::parseDispFreq, this, _1);

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
		std::string args;
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
			args = (firstspace == std::string::npos) ? "" : line.substr(firstspace + 1);
			try
			{
				//Call the appropriate parsing func
				m_parsers.at(command)(args);
			}
			catch(...)
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
		 << "\ndispfreq " << (int)dispfreq;

	file.close();

	return !file.fail();
}

void Config::writeError(const char *command)
{
	std::cerr << "Error while parsing configuration file : invalid value for \"" << command << "\" parameter." << std::endl;
}

bool Config::parseName(std::string &args)
{
	//Check that the name is correct
	if(args.empty())
	{
		writeError("name");
		return false;
	}

	name = std::move(args);
	return true;
}

bool Config::parseVideoMode(std::string &args)
{
	unsigned int tempw, temph;
	std::istringstream strm(args);//Parse args : wwww hhhh
	strm >> tempw >> temph;
	if(!strm || tempw < 800 || temph < 600)
	{
		writeError("videomode");
		return false;
	}

	width = tempw;
	height = temph;
	return true;
}

bool Config::parseFullscreen(std::string &args)
{
	int temp;
	std::istringstream strm(args);
	strm >> temp;
	if(!strm)
	{
		writeError("fullscreen");
		return false;
	}

	fullscreen = temp;
	return true;
}

bool Config::parseVSync(std::string &args)
{
	int temp;
	std::istringstream strm(args);
	strm >> temp;
	if(!strm)
	{
		writeError("vsync");
		return false;
	}

	vsync = temp;
	return true;
}

bool Config::parseDispFreq(std::string &args)
{
	unsigned int temp;
	std::istringstream strm(args);
	strm >> temp;
	if(!strm)
	{
		writeError("dispfreq");
		return false;
	}

	dispfreq = temp;
	return true;
}
