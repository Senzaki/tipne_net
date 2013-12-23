#ifndef RESOURCEMANAGER_HPP_INCLUDED
#define RESOURCEMANAGER_HPP_INCLUDED

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <list>
#include <string>

enum class ResourceSection : int
{
	Menu = 0,
	Count
};

class ResourceManager
{
	public:
	static ResourceManager &getInstance();

	~ResourceManager();

	ResourceManager(const ResourceManager &) = delete;
	ResourceManager &operator=(const ResourceManager &) = delete;

	void loadSection(ResourceSection name);
	void unloadSection(ResourceSection name);
	void unloadAllSections();
	bool isSectionLoaded(ResourceSection name) const;

	const sf::Texture &getTexture(ResourceSection section, const std::string &name) const;
	const sf::Font &getFont(ResourceSection section, const std::string &name) const;

	private:
	ResourceManager();

	struct Section
	{
		bool loaded;
		std::unordered_map<std::string, sf::Texture *> textures;
		std::unordered_map<std::string, sf::Font *> fonts;

		Section() : loaded(false) { }
	};

	std::vector<Section> m_sections;

	sf::Texture m_defaulttex;
	sf::Font m_defaultfont;
};

#endif // RESOURCEMANAGER_HPP_INCLUDED
