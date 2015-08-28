#include "nel_game.h"

#include <algorithm>
#include <cstdint>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>



namespace nel {



// --- TApplication ---

TApplication::TApplication()
:	IApplication(),
	appPath(),
	scenes(),
	scenesMutex(),
	logics(),
	logicsMutex(),
	factory(),
	state(),
	shouldQuit(false),
	eventHandlersMutex(),
	eventHandlers()
{
}

TApplication::~TApplication()
{
}

bool TApplication::initialize(const std::string& filename)
{
	// let descendants register factory functions etc
	beforeInitialization();

	// set application path
	appPath = filename;
	#ifdef WIN32
	std::replace(appPath.begin(), appPath.end(), '\\', '/');
	#endif
	size_t found = appPath.find_last_of('/');
	appPath = appPath.substr(0, found+1);

	// create window
	sf::RenderWindow* w = createWindow();
	if (!w)
		return false;
	window.reset(w);	// set our smart pointer to the newly created window

	// the initial game state
	IGameState* initialState = createInitialGameState();
	if (!initialState)
		return false;
	state.reset(initialState);
	state->initialize(this);

	// let descendants do stuff as well
	afterInitialization();

	// done
	return true;
}

void TApplication::finalize()
{
	// let descendants do stuff
	beforeFinalization();

	// finalize state
	if (state)
		state->finalize();

	// destroy state, window
	state.reset();
	window.reset();	
}

void TApplication::execute()
{
	sf::Clock clock;

	const int TICKS_PER_SECOND = 25;
    const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
    const int MAX_FRAMESKIP = 5;

	uint32_t startTickForFPS = clock.getElapsedTime().asMilliseconds();
	uint32_t nextGameTick = startTickForFPS;
    int loops = 0;
    //float interpolation;

	uint32_t prevTickTime = clock.getElapsedTime().asMilliseconds();
	uint32_t nowTime;

	while (window->isOpen())
	{
		// handle events
		sf::Event event;
		while (window->pollEvent(event))
		{
			// let the state handle the event first
			bool handled = state->processEvent(event);	
			// then all registered event handlers
			if (!handled)
			{
				std::lock_guard<std::mutex> g(eventHandlersMutex);
				for (auto it = eventHandlers.begin(); it != eventHandlers.end(); it++)
				{
					if ((*it)->processEvent(event))
						break;
				}
			}
		}

		// let the logic objects process a tick
        loops = 0;
		nowTime = clock.getElapsedTime().asMilliseconds();
        while (nowTime > nextGameTick && loops < MAX_FRAMESKIP) 
		{	
			TGameTime delta = nowTime-prevTickTime;
			state->update(delta);
			{				
				std::lock_guard<std::mutex> g(logicsMutex);
				for (auto it = logics.begin(); it != logics.end(); it++)
					(*it)->update(delta);
			}
			// get new time
            nextGameTick += SKIP_TICKS;
            loops++;
			prevTickTime = nowTime;
			nowTime = clock.getElapsedTime().asMilliseconds();
        }

		// draw scenes
		sf::RenderTarget* target = window.get();
		target->clear();	// fixed to black, for now
		{
			std::lock_guard<std::mutex> g(scenesMutex);
			for (auto it = scenes.begin(); it != scenes.end(); it++)
				(*it)->draw(target);
		}

		// paint
		beforeDisplay();
        window->display();
		afterDisplay();

		// this will most likely be set in reaction to some event, i.e. by the game state
		if (shouldQuit)
			window->close();
	}
}

void TApplication::setNextState(IGameState* nextState)
{
	if (state)
		state->finalize();

	state.reset(nextState);

	state->initialize(this);
}

void TApplication::attachScene(IScenePtr scene)
{
	std::lock_guard<std::mutex> g(scenesMutex);

	scene->onAttach(this);
	scenes.push_back(scene);

	afterSceneAttached(scene);
}

void TApplication::detachScene(IScenePtr scene)
{
	beforeSceneDetached(scene);

	std::lock_guard<std::mutex> g(scenesMutex);

	for (auto it = scenes.begin(); it != scenes.end(); it++)
	{
		if (*(it) == scene)
		{
			scenes.erase(it);
			scene->onDetach();
			break;
		}
	}
}

void TApplication::addLogic(ILogicPtr logic)
{
	std::lock_guard<std::mutex> g(logicsMutex);

	logics.push_back(logic);
}

void TApplication::removeLogic(ILogicPtr logic)
{
	std::lock_guard<std::mutex> g(logicsMutex);

	for (auto it = logics.begin(); it != logics.end(); it++)
	{
		if (*(it) == logic)
		{
			logics.erase(it);
			break;
		}
	}
}

void TApplication::addEventHandler(IEventHandler* handler)
{
	std::lock_guard<std::mutex> g(eventHandlersMutex);

	eventHandlers.push_back(handler);
}

void TApplication::removeEventHandler(IEventHandler* handler)
{
	std::lock_guard<std::mutex> g(eventHandlersMutex);

	for (auto it = eventHandlers.begin(); it != eventHandlers.end(); it++)
	{
		if (*(it) == handler)
		{
			eventHandlers.erase(it);
			break;
		}
	}	
}

void TApplication::beforeInitialization()
{
}

void TApplication::afterInitialization()
{
}

void TApplication::beforeFinalization()
{
}

void TApplication::beforeDisplay()
{
}

void TApplication::afterDisplay()
{
}

void TApplication::afterSceneAttached(IScenePtr scene)
{
}

void TApplication::beforeSceneDetached(IScenePtr scene)
{
}

void TApplication::requestQuit()
{
	shouldQuit = true;
}

};	// namespace nel
