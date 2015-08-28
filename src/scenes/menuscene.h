#pragma once

#include "../base/sfgui/nel_scene_sfgui.h"



class TMenuScene :	public nel::TSFGUIScene					
{
private:	
	void createControls();

	void onButtonClick();

public:
	TMenuScene();
	~TMenuScene() override;

	// from TSFGUIScene
	void onAttach(nel::IApplication* setApplication) override;
	void onDetach() override;
};