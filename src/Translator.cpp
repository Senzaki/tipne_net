#include "Translator.hpp"
#include "Config.hpp"
#include <fstream>
#include <iostream>

Translator &Translator::getInstance()
{
	static Translator translator;
	return translator;
}

Translator::Translator()
{

}

bool Translator::loadPackage(const char *name)
{
	std::string filename = "data/tr/" + Config::getInstance().lang + '/' + name + ".tr";
	std::ifstream file(filename);

	if(!file)
	{
		std::cerr << "Error : Could not open translation file \"" << filename << "\" for reading." << std::endl;
		std::cerr << "        (Is " << Config::getInstance().lang << " a valid language ?)" << std::endl;
		return false;
	}

	unsigned int linenum = 0;
	bool textcontinues;
	std::string idline;
	std::string textline;
	std::string curtext;

	//While there are text ids
	while(std::getline(file, idline))
	{
		linenum++;
		//Skip the line if it is empty
		if(idline.empty())
			continue;
		//Check that the id starts with a #
		if(idline[0] != '#')
		{
			std::cerr << "Error in translation file " << filename << " at line " << linenum << std::endl;
			std::cerr << "Text ID does not start with a #. Ignoring it." << std::endl;
			continue;
		}
		//Read the translated text until we reach end of text (line starting with a .)
		textcontinues = true;
		curtext.clear();
		while(textcontinues)
		{
			if(!std::getline(file, textline))
			{
				std::cout << "Error while reading translation file " << filename << " : EOF reached too soon." << std::endl;
				break;
			}
			linenum++;
			if(textline.empty())
			{
				std::cerr << "Error in translation file " << filename << " at line " << linenum << std::endl;
				std::cerr << "Text has to start either with a '+' or a '.'." << std::endl;
				break;
			}
			else if(textline[0] == '+')
			{
				//Add the text, next line will be for the same text
				curtext += textline.c_str() + 1;
				curtext += '\n';
			}
			else if(textline[0] == '.')
			{
				//Add the text and go to the next id
				curtext += textline.c_str() + 1;
				textcontinues = false;
			}
			else
			{
				std::cerr << "Error in translation file " << filename << " at line " << linenum << std::endl;
				std::cerr << "Text has to start either with a '+' or a '.'." << std::endl;
				break;
			}
		}
		//If there was no error, add the text
		if(!textcontinues)
			m_texts[idline.substr(1)] = curtext;
	}

	file.close();

	return true;
}

const std::string &Translator::translate(const std::string &id) const
{
	//Return the translated text if it exists, and return the key otherwise
	try
	{
		return m_texts.at(id);
	}
	catch(const std::out_of_range &)
	{
		std::cerr << "Error : Text ID \"" << id << "\" could not be found in the translation tables." << std::endl;
		return id;
	}
}
