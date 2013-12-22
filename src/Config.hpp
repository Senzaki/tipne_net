#ifndef CONFIG_HPP_INCLUDED
#define CONFIG_HPP_INCLUDED

#include <string>
#include <functional>
#include <unordered_map>

constexpr const char *DEFAULT_LANGUAGE = "en";

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

	private:
	Config();

	std::unordered_map<std::string, std::function<bool(const char *)>> m_parsers;

	void writeError(const char *command);

	bool parseName(const char *args);
	bool parseVideoMode(const char *args);
	bool parseFullscreen(const char *args);
	bool parseVSync(const char *args);
	bool parseDispFreq(const char *args);
	bool parseLanguage(const char *args);
};

#endif // CONFIG_HPP_INCLUDED
