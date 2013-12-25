#include "ResourceManager.hpp"
#include <iostream>

constexpr const char *TEXTURE_PATH = "data/textures/";
constexpr const char *FONT_PATH = "data/fonts/";

ResourceManager &ResourceManager::getInstance()
{
	static ResourceManager rsmgr;
	return rsmgr;
}

ResourceManager::ResourceManager()
{
	m_sections.resize((int)ResourceSection::Count);
	//Base
	m_sections[(int)ResourceSection::Base].textures["cursor.png"] = nullptr;
	//Menu
	m_sections[(int)ResourceSection::Menu].textures["default.png"] = nullptr;
	m_sections[(int)ResourceSection::Menu].fonts["ASafePlacetoFall.ttf"] = nullptr;
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
	for(auto &it : sec.fonts)
	{
		it.second = new sf::Font;
		it.second->loadFromFile(FONT_PATH + it.first);
	}
	//Load all textures
	for(auto &it : sec.textures)
	{
		it.second = new sf::Texture;
		it.second->loadFromFile(TEXTURE_PATH + it.first);
	}
	sec.loaded = true;
}

void ResourceManager::unloadSection(ResourceSection name)
{
	Section &sec = m_sections[(int)name];
	if(!sec.loaded)
		return;
	//Unload all fonts
	for(auto &it : sec.fonts)
	{
		delete it.second;
		it.second = nullptr;
	}
	//Unload all textures
	for(auto &it : sec.textures)
	{
		delete it.second;
		it.second = nullptr;
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
		for(auto &it : sec.fonts)
		{
			delete it.second;
			it.second = nullptr;
		}
		//Unload all textures
		for(auto &it : sec.textures)
		{
			delete it.second;
			it.second = nullptr;
		}
		sec.loaded = false;
	}
}

bool ResourceManager::isSectionLoaded(ResourceSection name) const
{
	return m_sections[(int)name].loaded;
}

const sf::Texture &ResourceManager::getTexture(ResourceSection section, const std::string &name) const
{
	const Section &sec = m_sections[(int)section];
	//Is the section loaded ?
	if(!sec.loaded)
	{
		std::cerr << "Error : Texture " << TEXTURE_PATH << name << " has to be loaded before it can be used." << std::endl;
		return m_defaulttex;
	}
	//Return the resource
	try
	{
		return *sec.textures.at(name);
	}
	catch(const std::out_of_range &)
	{
		std::cerr << "Error : Texture " << TEXTURE_PATH << name << " does not exist in this section." << std::endl;
		return m_defaulttex;
	}
}

const sf::Font &ResourceManager::getFont(ResourceSection section, const std::string &name) const
{
	const Section &sec = m_sections[(int)section];
	//Is the section loaded ?
	if(!sec.loaded)
	{
		std::cerr << "Error : Font " << FONT_PATH << name << " has to be loaded before it can be used." << std::endl;
		return m_defaultfont;
	}
	//Return the resource
	try
	{
		return *m_sections[(int)section].fonts.at(name);
	}
	catch(const std::out_of_range &)
	{
		std::cerr << "Error : Font " << FONT_PATH << name << " does not exist in this section." << std::endl;
		return m_defaultfont;
	}
}
