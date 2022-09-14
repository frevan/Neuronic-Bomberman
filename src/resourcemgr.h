#pragma once

#include <string>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "gamedata.h"

class TFontManager
{
private:
	std::string FontPath;
	sf::Font defaultFont;
public:
	enum
	{
		standard	= 0,

		count
	};

	TFontManager();

	void SetFontPath(const std::string& NewValue);
	void LoadFonts();
	const sf::Font &ByIndex(unsigned int index);
};

class TMapList
{
public:
	std::vector<TArena*> Maps;

	TMapList();
	~TMapList();

	void LoadAllMaps(const std::string& Path);
	void Clear();

	TArena* MapFromIndex(int Index);
	TArena* MapFromName(const std::string& Name);
};