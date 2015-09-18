#pragma once

#include "../nel_game.h"

#include <TGUI/TGUI.hpp>



namespace nel {

class TTGUIApplication :	public TApplication,
							public nel::IEventHandler
{
protected:
	std::shared_ptr<tgui::Gui> GUI;

	void AfterInitialization() override;
	void BeforeFinalization() override;
	void BeforeDisplay() override;
	void AfterSceneAttached(IViewPtr scene) override;
	void BeforeSceneDetached(IViewPtr scene) override;

	virtual std::string GetResourceSubPath() { return "Artwork/"; };
	virtual std::string GetFontSubPath() { return "Fonts/"; };
	virtual std::string GetDefaultFontName() = 0;

public:
	TTGUIApplication();

	// from IEventHandler
	bool ProcessEvent(const sf::Event& event) override;
};

};	// namespace nel