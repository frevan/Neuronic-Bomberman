#include "basegame.h"

#include <algorithm>
#include <cstdint>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>



// --- TGameFactory ---

TGameFactory::TGameFactory()
:	IObjectFactory(),
	delegates()
{
}

void TGameFactory::registerObjectType(uint64_t objectType, const TObjectFactoryDelegate& setDelegate)
{
	delegates[objectType] = setDelegate;
}

void TGameFactory::unregisterObjectType(uint64_t objectType)
{
	delegates.erase(objectType);
}

void* TGameFactory::createObject(uint64_t objectType)
{
	return delegates[objectType]();
}



// --- TGameApplication ---

TGameApplication::TGameApplication()
:	IApplication(),
	appPath(),
	scenes(),
	scenesMutex(),
	logics(),
	logicsMutex(),
	factory(),
	state()
{
}

TGameApplication::~TGameApplication()
{
}

bool TGameApplication::initialize(const std::string& filename)
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

	// the first game state
	state.reset((IGameState*)factory.createObject(guid_DefaultGameState));

	// let descendants do stuff as well
	afterInitialization();

	// done
	return true;
}

void TGameApplication::finalize()
{
	// let descendants do stuff
	beforeFinalization();

	// destroy state, window
	state.reset();
	window.reset();	
}

void TGameApplication::execute()
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
			// window closed or escape key pressed: exit
			if ((event.type == sf::Event::Closed) || ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
			{
				window->close();
				break;
			}
		}

		/*
		// check for messages
		while (processOneEvent())
		{
		}

		if (shouldQuit())	if (processQuitRequest())
			break;

		// process events
		TBaseEventManager::get()->process(20);	// process events for up to 20 milliseconds

		if (shouldQuit())	if (processQuitRequest())
			break;
		*/

		// let the logic objects process a tick
        loops = 0;
		nowTime = clock.getElapsedTime().asMilliseconds();
        while (nowTime > nextGameTick && loops < MAX_FRAMESKIP) 
		{	
			TGameTime delta = nowTime-prevTickTime;
			if (state)	
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

		//int curTicks = clock.getElapsedTime().asMilliseconds();

		// calculate fps
		//fpsCalculator->newFrame();

		// calculate interpolation
        //interpolation = float(curTicks + SKIP_TICKS - nextGameTick) / float(SKIP_TICKS);

		// draw views
		{
			sf::RenderTarget* target = window.get();

			target->clear();	// fixed to black, for now

			std::lock_guard<std::mutex> g(scenesMutex);
			for (auto it = scenes.begin(); it != scenes.end(); it++)
				(*it)->draw(target);
		}

		// paint		
        window->display();
	}
}

IGameState* TGameApplication::getState()
{
	// TODO
	return nullptr;
}

void TGameApplication::setState(IGameState* state)
{
	// TODO
}

void TGameApplication::attachScene(IScenePtr scene)
{
	std::lock_guard<std::mutex> g(scenesMutex);

	scene->onAttach();
	scenes.push_back(scene);
}

void TGameApplication::detachScene(IScenePtr scene)
{
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

void TGameApplication::addLogic(ILogicPtr logic)
{
	std::lock_guard<std::mutex> g(logicsMutex);

	logics.push_back(logic);
}

void TGameApplication::removeLogic(ILogicPtr logic)
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

void TGameApplication::beforeInitialization()
{
	factory.registerObjectType(guid_DefaultGameState, std::bind(&TGameApplication::createDummyGameState, this));
}

void TGameApplication::afterInitialization()
{
}

void TGameApplication::beforeFinalization()
{
}

void* TGameApplication::createDummyGameState()
{
	return new TDummyGameState();
}



// --- TDummyGameState ---

TDummyGameState::TDummyGameState()
:	IGameState(),
	application(nullptr)
{
}

void TDummyGameState::initialize(IApplication* setApplication)
{
	application = setApplication;
}

void TDummyGameState::finalize()
{
	application = nullptr;
}

void TDummyGameState::update(TGameTime deltaTime)
{
}