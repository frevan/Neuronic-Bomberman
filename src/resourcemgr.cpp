#include "resourcemgr.h"

TFontManager::TFontManager()
:	defaultFont(),
	FontPath()
{
}

void TFontManager::LoadFonts()
{
	defaultFont.loadFromFile(FontPath + "OpenSans-Regular.ttf");
}

const sf::Font& TFontManager::ByIndex(unsigned int index)
{
	switch (index)
	{
		case standard:
			return defaultFont;
			break;
		default:
			return defaultFont;
			break;
	}
}

void TFontManager::SetFontPath(const std::string& NewValue)
{
	FontPath = NewValue;
}
