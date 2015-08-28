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
	std::vector<IScenePtr> Scenes;
	std::mutex ScenesMutex;
	std::vector<ILogicPtr> Logics;
	std::mutex LogicsMutex;
	std::unique_ptr<IGameState> State;
	std::mutex EventHandlersMutex;
	std::vector<IEventHandler*> EventHandlers;

protected:
	std::unique_ptr<sf::RenderWindow> Window;

	virtual void BeforeInitialization();
	virtual void AfterInitialization();
	virtual void BeforeFinalization();
	virtual void BeforeDisplay();
	virtual void AfterDisplay();
	virtual void AfterSceneAttached(IScenePtr scene);
	virtual void BeforeSceneDetached(IScenePtr scene);

	virtual sf::RenderWindow* createWindow() = 0;
	virtual IGameState* createInitialGameState() = 0;

public:
	std::string AppPath;
	TObjectFactory Factory;

	TApplication();
	~TApplication() override;

	bool initialize(const std::string& filename);
	void finalize();
	virtual void execute();	

	// from IApplication
	void RequestQuit() override;
	void SetNextState(IGameState* nextState) override;
	void AttachScene(IScenePtr scene) override;
	void DetachScene(IScenePtr scene) override;
	void AddLogic(ILogicPtr logic) override;
	void RemoveLogic(ILogicPtr logic) override;
	void AddEventHandler(IEventHandler* handler) override;
	void RemoveEventHandler(IEventHandler* handler) override;
};

};	// namespace nel
