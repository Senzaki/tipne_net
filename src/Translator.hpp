#ifndef TRANSLATOR_HPP_INCLUDED
#define TRANSLATOR_HPP_INCLUDED

#include <string>
#include <unordered_map>

//Helper macro (shorter)
#define tr(x) Translator::getInstance().translate(x)

//The translator will use the language specified in Config::lang.
class Translator
{
	public:
	static Translator &getInstance();

	Translator(const Translator &) = delete;
	Translator &operator=(const Translator &) = delete;

	bool loadPackage(const char *name);

	const std::string &translate(const std::string &id) const;

	private:
	Translator();

	std::unordered_map<std::string, std::string> m_texts;
};

#endif // TRANSLATOR_HPP_INCLUDED
