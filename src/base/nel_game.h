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
	bool shouldQuit;
	std::vector<IScenePtr> scenes;
	std::mutex scenesMutex;
	std::vector<ILogicPtr> logics;
	std::mutex logicsMutex;
	std::unique_ptr<IGameState> state;
	std::mutex eventHandlersMutex;
	std::vector<IEventHandler*> eventHandlers;

protected:
	std::unique_ptr<sf::RenderWindow> window;

	virtual void beforeInitialization();
	virtual void afterInitialization();
	virtual void beforeFinalization();
	virtual void beforeDisplay();
	virtual void afterDisplay();
	virtual void afterSceneAttached(IScenePtr scene);
	virtual void beforeSceneDetached(IScenePtr scene);

	virtual sf::RenderWindow* createWindow() = 0;
	virtual IGameState* createInitialGameState() = 0;

public:
	std::string appPath;
	TObjectFactory factory;

	TApplication();
	~TApplication() override;

	bool initialize(const std::string& filename);
	void finalize();
	virtual void execute();	

	// from IApplication
	void requestQuit() override;
	void setNextState(IGameState* nextState) override;
	void attachScene(IScenePtr scene) override;
	void detachScene(IScenePtr scene) override;
	void addLogic(ILogicPtr logic) override;
	void removeLogic(ILogicPtr logic) override;
	void addEventHandler(IEventHandler* handler) override;
	void removeEventHandler(IEventHandler* handler) override;
};

};	// namespace nel
