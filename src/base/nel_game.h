#pragma once

#include <string>
#include <memory>
#include <mutex>

#include <SFML/Window.hpp>

#include "nel_interfaces.h"
#include "nel_factory.h"



namespace nel {

class TApplication :	public IApplication
{
private:	
	bool ShouldQuit;
	IFpsCalculator* FpsCalculator;
	std::vector<IScenePtr> Scenes;
	std::mutex ScenesMutex;
	std::vector<ILogicPtr> Logics;
	std::mutex LogicsMutex;	
	std::mutex EventHandlersMutex;
	std::vector<IEventHandler*> EventHandlers;
	std::unique_ptr<IGameState> CurrentState;
	TGameID NextStateID;

	std::string DetermineAppPath(const std::string& filename);
	void SwitchToNextState();

	// factory function(s)
	void* CreateFpsCalculator();

protected:
	std::unique_ptr<sf::RenderWindow> Window;

	virtual void BeforeInitialization();
	virtual void AfterInitialization();
	virtual void BeforeFinalization();
	virtual void BeforeDisplay();
	virtual void AfterDisplay();
	virtual void AfterSceneAttached(IScenePtr scene);
	virtual void BeforeSceneDetached(IScenePtr scene);

	virtual sf::RenderWindow* CreateWindow() = 0;
	virtual TGameID GetInitialGameStateID() = 0;
	virtual IGameState* CreateGameState(TGameID id) = 0;

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
	void SetNextState(TGameID id) override;
	void AttachScene(IScenePtr scene) override;
	void DetachScene(IScenePtr scene) override;
	void AddLogic(ILogicPtr logic) override;
	void RemoveLogic(ILogicPtr logic) override;
	void AddEventHandler(IEventHandler* handler) override;
	void RemoveEventHandler(IEventHandler* handler) override;
	double GetCurrentFps() override;
};

};	// namespace nel
