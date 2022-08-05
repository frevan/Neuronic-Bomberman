#pragma once

#include <string>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

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