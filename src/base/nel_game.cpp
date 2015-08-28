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
	AppPath(),
	Scenes(),
	ScenesMutex(),
	Logics(),
	LogicsMutex(),
	Factory(),
	State(),
	ShouldQuit(false),
	EventHandlersMutex(),
	EventHandlers(),
	Window()
{
}

TApplication::~TApplication()
{
}

bool TApplication::initialize(const std::string& filename)
{
	// let descendants register factory functions etc
	BeforeInitialization();

	// set application path
	AppPath = filename;
	#ifdef WIN32
	std::replace(AppPath.begin(), AppPath.end(), '\\', '/');
	#endif
	size_t found = AppPath.find_last_of('/');
	AppPath = AppPath.substr(0, found+1);

	// create window
	sf::RenderWindow* w = createWindow();
	if (!w)
		return false;
	Window.reset(w);	// set our smart pointer to the newly created window

	// let descendants do stuff as well
	AfterInitialization();

	// done
	return true;
}

void TApplication::finalize()
{
	// let descendants do stuff
	BeforeFinalization();

	// finalize state
	if (State)
		State->Finalize();

	// destroy state, window
	State.reset();
	Window.reset();	
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

	// one more thing left to create:
	// the initial game state
	IGameState* initialState = createInitialGameState();
	if (!initialState)
		return;
	State.reset(initialState);
	State->Initialize(this);

	while (Window->isOpen())
	{
		// handle events
		sf::Event event;
		while (Window->pollEvent(event))
		{
			// let the state handle the event first
			bool handled = State->ProcessEvent(event);	
			// then all registered event handlers
			if (!handled)
			{
				std::lock_guard<std::mutex> g(EventHandlersMutex);
				for (auto it = EventHandlers.begin(); it != EventHandlers.end(); it++)
				{
					if ((*it)->ProcessEvent(event))
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
			State->Update(delta);
			{				
				std::lock_guard<std::mutex> g(LogicsMutex);
				for (auto it = Logics.begin(); it != Logics.end(); it++)
				{
					auto lockedlogic = (*it).lock();
					if (lockedlogic)
						lockedlogic->Update(delta);
				}
			}
			// get new time
            nextGameTick += SKIP_TICKS;
            loops++;
			prevTickTime = nowTime;
			nowTime = clock.getElapsedTime().asMilliseconds();
        }

		// draw scenes
		sf::RenderTarget* target = Window.get();
		target->clear();	// fixed to black, for now
		{
			std::lock_guard<std::mutex> g(ScenesMutex);
			for (auto it = Scenes.begin(); it != Scenes.end(); it++)
			{
				auto lockedscene = (*it).lock();
				if (lockedscene)
					lockedscene->Draw(target);
			}
		}

		// paint
		BeforeDisplay();
        Window->display();
		AfterDisplay();

		// this will most likely be set in reaction to some event, i.e. by the game state
		if (ShouldQuit)
			Window->close();
	}
}

void TApplication::SetNextState(IGameState* nextState)
{
	if (State)
		State->Finalize();

	State.reset(nextState);

	State->Initialize(this);
}

void TApplication::AttachScene(IScenePtr scene)
{
	std::lock_guard<std::mutex> g(ScenesMutex);

	auto lockedscene = scene.lock();
	if (lockedscene)
		lockedscene->OnAttach(this);

	Scenes.push_back(scene);

	AfterSceneAttached(scene);
}

void TApplication::DetachScene(IScenePtr scene)
{
	BeforeSceneDetached(scene);

	std::lock_guard<std::mutex> g(ScenesMutex);

	for (auto it = Scenes.begin(); it != Scenes.end(); it++)
	{
		auto lockeditem = (*it).lock();
		auto lockedscene = scene.lock();
		if (lockeditem == lockedscene)
		{
			Scenes.erase(it);
			lockedscene->OnDetach();
			break;
		}
	}
}

void TApplication::AddLogic(ILogicPtr logic)
{
	std::lock_guard<std::mutex> g(LogicsMutex);

	Logics.push_back(logic);
}

void TApplication::RemoveLogic(ILogicPtr logic)
{
	std::lock_guard<std::mutex> g(LogicsMutex);

	for (auto it = Logics.begin(); it != Logics.end(); it++)
	{
		auto lockeditem = (*it).lock();
		auto lockedlogic = logic.lock();
		if (lockeditem == lockedlogic)
		{
			Logics.erase(it);
			break;
		}
	}
}

void TApplication::AddEventHandler(IEventHandler* handler)
{
	std::lock_guard<std::mutex> g(EventHandlersMutex);

	EventHandlers.push_back(handler);
}

void TApplication::RemoveEventHandler(IEventHandler* handler)
{
	std::lock_guard<std::mutex> g(EventHandlersMutex);

	for (auto it = EventHandlers.begin(); it != EventHandlers.end(); it++)
	{
		if (*(it) == handler)
		{
			EventHandlers.erase(it);
			break;
		}
	}	
}

void TApplication::BeforeInitialization()
{
}

void TApplication::AfterInitialization()
{
}

void TApplication::BeforeFinalization()
{
}

void TApplication::BeforeDisplay()
{
}

void TApplication::AfterDisplay()
{
}

void TApplication::AfterSceneAttached(IScenePtr scene)
{
}

void TApplication::BeforeSceneDetached(IScenePtr scene)
{
}

void TApplication::RequestQuit()
{
	ShouldQuit = true;
}

};	// namespace nel
