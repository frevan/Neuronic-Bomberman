#include "resourcemgr.h"

#include <filesystem>

// --- TFontManager ---

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

// --- TMapList ---

TMapList::TMapList()
:	Maps()
{
}

TMapList::~TMapList()
{
	Clear();
}

void TMapList::Clear()
{
	for (auto it = Maps.begin(); it != Maps.end(); it++)
		delete (*it);

	Maps.clear();
}

void TMapList::LoadAllMaps(const std::string& Path)
{
	Clear();

	// find all maps
	std::list<std::filesystem::path> fileList;
	std::filesystem::path p(Path);
	for (const auto& file : std::filesystem::directory_iterator(p))
		fileList.push_back(file.path());

	size_t total = fileList.size();

	// load each one
	int idx = 0;
	for (auto it = fileList.begin(); it != fileList.end(); ++it)
	{
		std::filesystem::path path = (*it);

		// --> create map object and add it
		TArena* m = new TArena();
		std::string fname = it->string();
		if (m->LoadFromFile(fname, 0))
		{
			std::transform(m->OriginalFileName.begin(), m->OriginalFileName.end(), m->OriginalFileName.begin(), ::tolower);
			Maps.push_back(m);
		}
		else
			delete m;

		idx++;
	}
}

TArena* TMapList::MapFromIndex(int Index)
{
	int counter = 0;
	for (auto it = Maps.begin(); it != Maps.end(); it++)
	{
		if (counter == Index)
			return (*it);
		counter++;
	}

	return nullptr;
}