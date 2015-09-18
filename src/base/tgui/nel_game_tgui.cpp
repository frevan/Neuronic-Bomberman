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

void TTGUIApplication::AfterSceneAttached(IViewPtr scene)
{
	TApplication::AfterSceneAttached(scene);
}

void TTGUIApplication::BeforeSceneDetached(IViewPtr scene)
{
	TApplication::BeforeSceneDetached(scene);
}

bool TTGUIApplication::ProcessEvent(const sf::Event& event)
{
	GUI->handleEvent(event);
	return true;
}

};	// namespace nel