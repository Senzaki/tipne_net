#ifndef RESOURCEMANAGER_HPP_INCLUDED
#define RESOURCEMANAGER_HPP_INCLUDED

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum class ResourceSection
{
	Base = 0,
	Menu,
	Map,
	Game,
	Count
};

namespace Resource
{
	enum BaseTexture
	{
		CURSOR_TEX = 0,
		BUTTON_STANDARD_TEX,
		BUTTON_CLICKED_TEX,
		BUTTON_HIGHLIGHTED_TEX,
		CHECKBOX_CHECKED_TEX,
		CHECKBOX_UNCHECKED_TEX,
		LINEEDIT_STANDARD_TEX,
		BASE_TEXTURES_COUNT
	};

	enum BaseFont
	{
		STANDARD_FONT = 0,
		BASE_FONTS_COUNT
	};

	enum MenuTexture
	{
		LOGO_TEX = 0,
		MENU_TEXTURES_COUNT
	};

	enum MapTexture
	{
		BASE_TILES_TEX = 0,
		BASE_WALLS_TEX,
		MAP_TEXTURES_COUNT
	};

	enum GameTexture
	{
		GHOST_TEX = 0,
		BASIC_SPELL,
		GAME_TEXTURES_COUNT
	};
}

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

	const sf::Texture &getTexture(ResourceSection section, unsigned int name) const;
	const sf::Font &getFont(ResourceSection section, unsigned int name) const;

	private:
	ResourceManager();

	struct Section
	{
		bool loaded;
		std::vector<sf::Texture *> textures;
		std::vector<sf::Font *> fonts;

		std::vector<std::string> tex_files;
		std::vector<std::string> font_files;

		Section() : loaded(false) { }
	};

	std::vector<Section> m_sections;

	sf::Texture m_defaulttex;
	sf::Font m_defaultfont;
};

#endif // RESOURCEMANAGER_HPP_INCLUDED
