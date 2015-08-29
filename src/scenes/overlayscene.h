#pragma once

#include "../base/nel_interfaces.h"
#include "../base/tgui/nel_scene_tgui.h"



class TOverlayScene :	public nel::TTGUIScene
{
private:
	void CreateWidgets() override;

public:
	TOverlayScene(std::shared_ptr<tgui::Gui> setGUI);
	~TOverlayScene() override;

	// from IScene
	void Draw(sf::RenderTarget* target) override;
};
