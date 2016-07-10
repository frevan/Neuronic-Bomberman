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
	Views(),
	ViewsMutex(),
	Logics(),
	LogicsMutex(),
	Factory(),	
	ShouldQuit(false),
	EventHandlersMutex(),
	EventHandlers(),
	Window(),
	FpsCalculator(nullptr),
	GameState()
{
}

TApplication::~TApplication()
{
}

bool TApplication::Initialize(const std::string& filename)
{
	Factory.RegisterObjectType(OT_FPSCalculator, std::bind(&TApplication::CreateFpsCalculator, this));
	Factory.RegisterObjectType(OT_StateMachine, std::bind(&TApplication::CreateStateMachine, this));

	BeforeInitialization();

	AppPath = DetermineAppPath(filename);

	sf::RenderWindow* w = CreateWindow();
	if (!w)
		return false;
	Window.reset(w);

	FpsCalculator = (IFpsCalculator*)Factory.CreateObject(OT_FPSCalculator);
	GameState = (IStateMachine*)Factory.CreateObject(OT_StateMachine);

	AfterInitialization();

	return true;
}

void TApplication::Finalize()
{
	BeforeFinalization();

	GameState->Finalize();

	Window.reset();	

	delete GameState;
	GameState = nullptr;
	delete FpsCalculator;
	FpsCalculator = nullptr;
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

	// get the initial state id ... switch to it in the loop
	GameState->SetNextState(GetInitialGameStateID());

	while (Window->isOpen())
	{
		GameState->SwitchToNextState();

		// handle events
		sf::Event event;
		while (Window->pollEvent(event))
		{
			// all registered event handlers
			std::lock_guard<std::mutex> g(EventHandlersMutex);
			for (auto it = EventHandlers.rbegin(); it != EventHandlers.rend(); it++)
			{
				auto eh = it->lock();
				if (eh)	if (eh->ProcessEvent(event))
					break;
			}
		}

		// let the logic objects process a tick
        loops = 0;
		nowTime = clock.getElapsedTime().asMilliseconds();
        while (nowTime > nextGameTick && loops < MAX_FRAMESKIP) 
		{	
			TGameTime delta = nowTime-prevTickTime;			
			// create tempo copy (so we can add new logics from inside the loop)  ---  better solution might be to create jobs for adding and removing logics?
			std::vector<ILogicPtr> tempLogics;
			{
				std::lock_guard<std::mutex> g(LogicsMutex);
				for (auto it = Logics.begin(); it != Logics.end(); it++)
					tempLogics.push_back(*it);
			}
			// process all (old) logics
			for (auto it = tempLogics.rbegin(); it != tempLogics.rend(); it++)
			{
				auto lockedlogic = (*it).lock();
				if (lockedlogic)
					lockedlogic->Update(delta);
			}
			// get new time
            nextGameTick += SKIP_TICKS;
            loops++;
			prevTickTime = nowTime;
			nowTime = clock.getElapsedTime().asMilliseconds();
        }		

		// draw views
		sf::RenderTarget* target = Window.get();		
		target->clear();	// fixed to black, for now
		BeforeDraw(target);
		{
			std::lock_guard<std::mutex> g(ViewsMutex);
			for (auto it = Views.rbegin(); it != Views.rend(); it++)
			{
				auto lockedview = (*it).lock();
				if (lockedview) if (lockedview->IsVisible())
					lockedview->Draw(target);
			}
		}
		AfterDraw(target);

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

void TApplication::AttachView(IViewPtr view)
{
	std::lock_guard<std::mutex> g(ViewsMutex);

	IView::TViewType viewtype = IView::VT_SYSTEMVIEW;

	auto lockedview = view.lock();
	if (lockedview)
	{
		viewtype = lockedview->GetType();
		lockedview->OnAttach(this);
	}

	auto insertat = Views.end();
	if (viewtype == IView::VT_OVERLAY || viewtype == IView::VT_SYSTEMVIEW)
	{
		for (auto it = Views.begin(); it != Views.end(); it++)
		{
			auto lockeditem = (*it).lock();
			if (lockeditem->GetType() > viewtype)
			{
				insertat = it;
				break;
			}
		}
	}

	Views.insert(insertat, view);

	AfterSceneAttached(view);
}

void TApplication::DetachView(IViewPtr view)
{
	BeforeSceneDetached(view);

	std::lock_guard<std::mutex> g(ViewsMutex);

	for (auto it = Views.begin(); it != Views.end(); it++)
	{
		auto lockeditem = (*it).lock();
		auto lockedview = view.lock();
		if (lockeditem == lockedview)
		{
			Views.erase(it);
			lockedview->OnDetach();
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

void TApplication::AddEventHandler(IEventHandlerPtr handler)
{
	std::lock_guard<std::mutex> g(EventHandlersMutex);

	EventHandlers.push_back(handler);
}

void TApplication::RemoveEventHandler(IEventHandlerPtr handler)
{
	std::lock_guard<std::mutex> g(EventHandlersMutex);

	for (auto it = EventHandlers.begin(); it != EventHandlers.end(); it++)
	{
		auto eh = handler.lock();
		auto eh2 = (it)->lock();

		auto ehp = eh.get();
		auto ehp2 = eh2.get();

		if (ehp == ehp2)
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

void TApplication::BeforeDraw(sf::RenderTarget* target)
{
}

void TApplication::AfterDraw(sf::RenderTarget* target)
{
}

void TApplication::BeforeDisplay()
{
}

void TApplication::AfterDisplay()
{
}

void TApplication::AfterSceneAttached(IViewPtr scene)
{
}

void TApplication::BeforeSceneDetached(IViewPtr scene)
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

void* TApplication::CreateStateMachine() 
{	
	return new TStateMachine(this); 
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

std::string TApplication::GetFontFileName(const std::string& FontIdentifier)
{
	return "";
}

};	// namespace nel
