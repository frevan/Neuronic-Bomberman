#include "nel_game_tgui.h"



namespace nel {

TTGUIApplication::TTGUIApplication()
:	TApplication(),
	GUI(nullptr)
{
}

void TTGUIApplication::AfterInitialization()
{
	TApplication::AfterInitialization();

	GUI = std::make_shared<tgui::Gui>(*Window);

	tgui::setResourcePath(AppPath + GetResourceSubPath());
	GUI->setGlobalFont(GetFontSubPath() + GetDefaultFontName());

	AddEventHandler(this);
}

void TTGUIApplication::BeforeFinalization()
{
	RemoveEventHandler(this);

	GUI.reset();
}

void TTGUIApplication::BeforeDisplay()
{
	GUI->draw();
}

void TTGUIApplication::AfterSceneAttached(IScenePtr scene)
{
	TApplication::AfterSceneAttached(scene);
}

void TTGUIApplication::BeforeSceneDetached(IScenePtr scene)
{
	TApplication::BeforeSceneDetached(scene);
}

bool TTGUIApplication::ProcessEvent(const sf::Event& event)
{
	GUI->handleEvent(event);
	return true;
}

};	// namespace nel