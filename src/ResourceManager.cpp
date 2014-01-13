#include "ResourceManager.hpp"
#include <iostream>

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
		sec.textures.resize(BASE_TEXTURES_COUNT, nullptr);
		sec.font_files.resize(BASE_FONTS_COUNT);
		sec.fonts.resize(BASE_FONTS_COUNT, nullptr);

		sec.tex_files[CURSOR_TEX] = "cursor.png";
		sec.tex_files[BUTTON_STANDARD_TEX] = "btn_bg.png";
		sec.tex_files[BUTTON_CLICKED_TEX] = "btn_cl.png";
		sec.tex_files[BUTTON_HIGHLIGHTED_TEX] = "btn_hl.png";
		sec.font_files[STANDARD_FONT] = "DejaVuSansMono.ttf";
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
		sec.fonts[i] = new sf::Font;
		sec.fonts[i]->loadFromFile(FONT_PATH + sec.font_files[i]);
	}
	//Load all textures
	for(unsigned int i = 0 ; i < sec.textures.size(); i++)
	{
		sec.textures[i] = new sf::Texture;
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
	for(sf::Font *&font : sec.fonts)
	{
		delete font;
		font = nullptr;
	}
	//Unload all textures
	for(sf::Texture *&tex : sec.textures)
	{
		delete tex;
		tex = nullptr;
	}
	sec.loaded = false;
}

void ResourceManager::unloadAllSections()
{
	for(Section &sec : m_sections)
	{
		if(!sec.loaded)
			continue;
		//Unload all fonts
		for(sf::Font *&font : sec.fonts)
		{
			delete font;
			font = nullptr;
		}
		//Unload all textures
		for(sf::Texture *&tex : sec.textures)
		{
			delete tex;
			tex = nullptr;
		}
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
		std::cerr << "Error : Texture " << TEXTURE_PATH << name << " has to be loaded before it can be used." << std::endl;
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
		std::cerr << "Error : Font " << FONT_PATH << name << " has to be loaded before it can be used." << std::endl;
		return m_defaultfont;
	}
	return *sec.fonts[name];
}
