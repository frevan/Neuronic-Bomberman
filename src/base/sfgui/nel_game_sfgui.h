#pragma once

#include "../nel_game.h"

#include <SFGUI/SFGUI.hpp>



namespace nel {

class TSFGUIApplication :	public TApplication
{
protected:
	sfg::SFGUI SFGUI;

	void afterInitialization() override;
	void beforeDisplay() override;
	void afterSceneAttached(IScenePtr scene) override;

public:
	TSFGUIApplication();
};

};	// namespace nel