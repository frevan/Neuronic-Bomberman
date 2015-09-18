#pragma once

#include <string>
#include <memory>
#include <mutex>

#include <SFML/Window.hpp>

#include "nel_interfaces.h"
#include "nel_factory.h"
#include "utility/nel_statemachine.h"



namespace nel {

class TApplication :	public IApplication
{
private:	
	bool ShouldQuit;
	IFpsCalculator* FpsCalculator;
	std::vector<IViewPtr> Scenes;
	std::mutex ScenesMutex;
	std::vector<ILogicPtr> Logics;
	std::mutex LogicsMutex;	
	std::mutex EventHandlersMutex;
	std::vector<IEventHandler*> EventHandlers;	

	std::string DetermineAppPath(const std::string& filename);

	// factory function(s)
	void* CreateFpsCalculator();
	void* CreateStateMachine();

protected:
	std::unique_ptr<sf::RenderWindow> Window;
	TStateMachine* StateMachine;

	virtual void BeforeInitialization();
	virtual void AfterInitialization();
	virtual void BeforeFinalization();
	virtual void BeforeDisplay();
	virtual void AfterDisplay();
	virtual void AfterSceneAttached(IViewPtr scene);
	virtual void BeforeSceneDetached(IViewPtr scene);

	virtual sf::RenderWindow* CreateWindow() = 0;
	virtual TGameID GetInitialGameStateID() = 0;

public:
	std::string AppPath;
	TObjectFactory Factory;

	TApplication();
	~TApplication() override;

	bool Initialize(const std::string& filename);
	void Finalize();
	virtual void Execute();	

	// from IApplication
	IObjectFactory& GetFactory() override;
	void RequestQuit() override;
	void AttachScene(IViewPtr scene) override;
	void DetachScene(IViewPtr scene) override;
	void AddLogic(ILogicPtr logic) override;
	void RemoveLogic(ILogicPtr logic) override;
	void AddEventHandler(IEventHandler* handler) override;
	void RemoveEventHandler(IEventHandler* handler) override;
	double GetCurrentFps() override;
};

};	// namespace nel
