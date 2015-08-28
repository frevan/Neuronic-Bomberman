#include "nel_game_sfgui.h"



namespace nel {

TSFGUIApplication::TSFGUIApplication()
:	TApplication(),
	SFGUI()
{
}

void TSFGUIApplication::afterInitialization()
{
	TApplication::afterInitialization();

	// We're not using SFML to render anything in this program, so reset OpenGL states. Otherwise we wouldn't see anything.
	// Maybe do this somewhere else?
	window->resetGLStates();
}

void TSFGUIApplication::beforeDisplay()
{
	SFGUI.Display(*window.get());
}

void TSFGUIApplication::afterSceneAttached(IScenePtr scene)
{
	window->resetGLStates();
}

};	// namespace nel