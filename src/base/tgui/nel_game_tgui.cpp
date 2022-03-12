#include "nel_game_tgui.h"

#include "../utility/nel_eventhandler.h"



namespace nel {

TTGUIApplication::TTGUIApplication()
:	TApplication(),
	GUI(nullptr),
	GUIView(nullptr)
{
	EventHandler = std::make_shared<TEventHandler>(IEventHandler::Type::APPLICATION, std::bind(&TTGUIApplication::ProcessEvent, this, std::placeholders::_1), nullptr);
}

void TTGUIApplication::AfterInitialization()
{
	TApplication::AfterInitialization();

	GUI = std::make_shared<tgui::Gui>(*Window);

	tgui::setResourcePath(AppPath + GetResourceSubPath());
	GUI->setFont(GetFontFileName());

	GUIView = std::make_shared<TTGUISystemView>(GUI, GameState);
	AttachView(GUIView);

	AddEventHandler(EventHandler);
}

void TTGUIApplication::BeforeFinalization()
{
	DetachView(GUIView);

	RemoveEventHandler(EventHandler);
	EventHandler.reset();

	GUI.reset();
}

void TTGUIApplication::AfterDraw(sf::RenderTarget* target)
{
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

std::string TTGUIApplication::GetFontFileName(const std::string& FontIdentifier)
{
	std::string name = FontIdentifier;
	if (FontIdentifier == "")	name = GetDefaultFontName();

	return AppPath + GetResourceSubPath() + GetFontSubPath() + name;
}

};	// namespace nel