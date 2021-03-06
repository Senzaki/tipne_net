#include "ResourceManager.hpp"
#include <iostream>
#include "make_unique.hpp"

static constexpr const char *TEXTURE_PATH = "data/textures/";
static constexpr const char *FONT_PATH = "data/fonts/";

ResourceManager &ResourceManager::getInstance()
{
	static ResourceManager rsmgr;
	return rsmgr;
}

ResourceManager::ResourceManager()
{
	using namespace Resource;

	m_sections.resize((int)ResourceSection::Count);

	//Base
	{
		Section &sec = m_sections[(int)ResourceSection::Base];
		sec.tex_files.resize(BASE_TEXTURES_COUNT);
		sec.textures.resize(BASE_TEXTURES_COUNT);
		sec.font_files.resize(BASE_FONTS_COUNT);
		sec.fonts.resize(BASE_FONTS_COUNT);

		sec.tex_files[CURSOR_TEX] = "cursor.png";
		sec.tex_files[BUTTON_STANDARD_TEX] = "btn_bg.png";
		sec.tex_files[BUTTON_CLICKED_TEX] = "btn_cl.png";
		sec.tex_files[BUTTON_HIGHLIGHTED_TEX] = "btn_hl.png";
		sec.tex_files[CHECKBOX_CHECKED_TEX] = "cb_cck.png";
		sec.tex_files[CHECKBOX_UNCHECKED_TEX] = "cb_unc.png";
		sec.tex_files[LINEEDIT_STANDARD_TEX] = "line_def.png";
		sec.font_files[STANDARD_FONT] = "DejaVuSansMono.ttf";
	}

	//Menu
	{
		Section &sec = m_sections[(int)ResourceSection::Menu];
		sec.tex_files.resize(MENU_TEXTURES_COUNT);
		sec.textures.resize(MENU_TEXTURES_COUNT);

		sec.tex_files[LOGO_TEX] = "logo.png";
	}

	//Map
	{
		Section &sec = m_sections[(int)ResourceSection::Map];
		sec.tex_files.resize(MAP_TEXTURES_COUNT);
		sec.textures.resize(MAP_TEXTURES_COUNT);

		sec.tex_files[BASE_TILES_TEX] = "tiles/base.png";
		sec.tex_files[BASE_WALLS_TEX] = "tiles/walls.png";
	}

	//Game
	{
		Section &sec = m_sections[(int)ResourceSection::Game];
		sec.tex_files.resize(GAME_TEXTURES_COUNT);
		sec.textures.resize(GAME_TEXTURES_COUNT);

		sec.tex_files[GHOST_TEX] = "ghost.png";
		sec.tex_files[BASIC_SPELL] = "spells/basic.png";
	}
}

ResourceManager::~ResourceManager()
{
	unloadAllSections();
}

void ResourceManager::loadSection(ResourceSection name)
{
	Section &sec = m_sections[(int)name];
	if(sec.loaded)
		return;
	//Load all fonts
	for(unsigned int i = 0 ; i < sec.fonts.size(); i++)
	{
		sec.fonts[i] = make_unique<sf::Font>();
		sec.fonts[i]->loadFromFile(FONT_PATH + sec.font_files[i]);
	}
	//Load all textures
	for(unsigned int i = 0 ; i < sec.textures.size(); i++)
	{
		sec.textures[i] = make_unique<sf::Texture>();
		sec.textures[i]->loadFromFile(TEXTURE_PATH + sec.tex_files[i]);
	}
	sec.loaded = true;
}

void ResourceManager::unloadSection(ResourceSection name)
{
	Section &sec = m_sections[(int)name];
	if(!sec.loaded)
		return;
	//Unload all fonts
	for(std::unique_ptr<sf::Font> &font : sec.fonts)
		font.reset();
	//Unload all textures
	for(std::unique_ptr<sf::Texture> &tex : sec.textures)
		tex.reset();
	sec.loaded = false;
}

void ResourceManager::unloadAllSections()
{
	for(Section &sec : m_sections)
	{
		if(!sec.loaded)
			continue;
		//Unload all fonts
		for(std::unique_ptr<sf::Font> &font : sec.fonts)
			font.reset();
		//Unload all textures
		for(std::unique_ptr<sf::Texture> &tex : sec.textures)
			tex.reset();
		sec.loaded = false;
	}
}

bool ResourceManager::isSectionLoaded(ResourceSection name) const
{
	return m_sections[(int)name].loaded;
}

const sf::Texture &ResourceManager::getTexture(ResourceSection section, unsigned int name) const
{
	const Section &sec = m_sections[(int)section];
	//Is the section loaded ?
	if(!sec.loaded)
	{
		std::cerr << "Error : Texture " << TEXTURE_PATH << sec.tex_files[name] << " has to be loaded before it can be used." << std::endl;
		return m_defaulttex;
	}
	return *sec.textures[name];
}

const sf::Font &ResourceManager::getFont(ResourceSection section, unsigned int name) const
{
	const Section &sec = m_sections[(int)section];
	//Is the section loaded ?
	if(!sec.loaded)
	{
		std::cerr << "Error : Font " << FONT_PATH << sec.font_files[name] << " has to be loaded before it can be used." << std::endl;
		return m_defaultfont;
	}
	return *sec.fonts[name];
}
