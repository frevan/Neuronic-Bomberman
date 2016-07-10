#pragma once

#include "../nel_game.h"

#include <TGUI/TGUI.hpp>

#include "nel_view_tgui.h"


namespace nel {

class TTGUIApplication :	public TApplication
{
private:
	std::shared_ptr<nel::IEventHandler> EventHandler;
	std::shared_ptr<TTGUISystemView> GUIView;
protected:
	std::shared_ptr<tgui::Gui> GUI;

	void AfterInitialization() override;
	void BeforeFinalization() override;
	void AfterDraw(sf::RenderTarget* target) override;
	void AfterSceneAttached(IViewPtr scene) override;
	void BeforeSceneDetached(IViewPtr scene) override;

	virtual std::string GetResourceSubPath() { return "Artwork/"; };
	virtual std::string GetFontSubPath() { return "Fonts/"; };
	virtual std::string GetDefaultFontName() = 0;

public:
	TTGUIApplication();

	virtual bool ProcessEvent(const sf::Event& event);

	// from IApplication
	std::string GetFontFileName(const std::string& FontIdentifier = "") override;
};

};	// namespace nel