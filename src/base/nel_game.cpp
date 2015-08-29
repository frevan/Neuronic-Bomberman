#include "nel_game.h"

#include <algorithm>
#include <cstdint>
#include <cassert>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "nel_objecttypes.h"
#include "utility/nel_fps.h"



namespace nel {

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
	Window(),
	FpsCalculator(nullptr)
{
}

TApplication::~TApplication()
{
}

bool TApplication::Initialize(const std::string& filename)
{
	Factory.RegisterObjectType(OT_FPSCalculator, std::bind(&TApplication::CreateFpsCalculator, this));

	BeforeInitialization();

	AppPath = DetermineAppPath(filename);

	sf::RenderWindow* w = CreateWindow();
	if (!w)
		return false;
	Window.reset(w);

	FpsCalculator = (IFpsCalculator*)Factory.CreateObject(OT_FPSCalculator);

	AfterInitialization();

	return true;
}

void TApplication::Finalize()
{
	BeforeFinalization();

	if (State)
		State->Finalize();	

	State.reset();
	Window.reset();	

	delete FpsCalculator;
}

void TApplication::Execute()
{
	sf::Clock clock;

	const int TICKS_PER_SECOND = 25;
    const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
    const int MAX_FRAMESKIP = 5;

	uint32_t startTickForFPS = clock.getElapsedTime().asMilliseconds();
	uint32_t nextGameTick = startTickForFPS;
    int loops = 0;

	uint32_t prevTickTime = clock.getElapsedTime().asMilliseconds();
	uint32_t nowTime;

	// one more thing left to create:
	// the initial game state
	IGameState* initialState = CreateGameState(GetInitialGameStateID());
	assert(initialState);
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

		if (FpsCalculator)
			FpsCalculator->NewFrame();

		// paint
		BeforeDisplay();
        Window->display();
		AfterDisplay();

		// this will most likely be set in reaction to some event, i.e. by the game state
		if (ShouldQuit)
			Window->close();
	}
}

IObjectFactory& TApplication::GetFactory()
{
	return Factory;
}

void TApplication::SetNextState(TGameID nextState)
{
	if (State)
		State->Finalize();

	State.reset(CreateGameState(nextState));
	assert(State);

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

void* TApplication::CreateFpsCalculator() 
{	
	return new TFpsCalculator(); 
}

std::string TApplication::DetermineAppPath(const std::string& filename)
{
	std::string path = filename;
	#ifdef WIN32
	std::replace(path.begin(), path.end(), '\\', '/');
	#endif
	size_t found = path.find_last_of('/');
	path = path.substr(0, found+1);

	return path;
}

double TApplication::GetCurrentFps()
{
	if (FpsCalculator)
		return FpsCalculator->Value;
	else 
		return 0.f;
}

};	// namespace nel
