#pragma once

#include "../base/nel_interfaces.h"
#include "../base/tgui/nel_scene_tgui.h"



class TMenuScene :	public nel::TTGUIScene
{
private:	
	void createControls();

	void OnNewGameBtnClick();
	void OnJoinGameBtnClick();
	void OnOptionsBtnClick();
	void OnQuitBtnClick();

public:
	TMenuScene(std::shared_ptr<tgui::Gui> setGUI);
	~TMenuScene() override;

	// from IScene
	void OnAttach(nel::IApplication* setApplication) override;
	void OnDetach() override;
	void Draw(sf::RenderTarget* target) override;
};