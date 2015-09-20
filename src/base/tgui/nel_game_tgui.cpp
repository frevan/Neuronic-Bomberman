#include "nel_game_tgui.h"

#include "../utility/nel_eventhandler.h"



namespace nel {

TTGUIApplication::TTGUIApplication()
:	TApplication(),
	GUI(nullptr)
{
	EventHandler = std::make_shared<TEventHandler>(IEventHandler::APPLICATION, std::bind(&TTGUIApplication::ProcessEvent, this, std::placeholders::_1), nullptr);
}

void TTGUIApplication::AfterInitialization()
{
	TApplication::AfterInitialization();

	GUI = std::make_shared<tgui::Gui>(*Window);

	tgui::setResourcePath(AppPath + GetResourceSubPath());
	GUI->setGlobalFont(GetFontSubPath() + GetDefaultFontName());

	AddEventHandler(EventHandler);
}

void TTGUIApplication::BeforeFinalization()
{
	RemoveEventHandler(EventHandler);
	EventHandler.reset();

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