#ifndef CONFIG_HPP_INCLUDED
#define CONFIG_HPP_INCLUDED

#include <string>
#include <functional>
#include <unordered_map>

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

	private:
	Config();

	std::unordered_map<std::string, std::function<bool(std::string &)>> m_parsers;

	void writeError(const char *command);

	bool parseName(std::string &args);
	bool parseVideoMode(std::string &args);
	bool parseFullscreen(std::string &args);
	bool parseVSync(std::string &args);
	bool parseDispFreq(std::string &args);
};

#endif // CONFIG_HPP_INCLUDED
