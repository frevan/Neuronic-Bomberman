#pragma once

#include <string>
#include <memory>
#include <mutex>

#include <SFML/Window.hpp>

#include "interfaces.h"



// the uid of the game state that's created when the game is started (in initialization)
// register your own in the application's beforeInitialize() function to customize this
const uint64_t guid_DefaultGameState = 0xEF84194F58674E36;



class TGameFactory :	public IObjectFactory
{
private:
	std::map<uint64_t, TObjectFactoryDelegate> delegates;
public:
	TGameFactory();

	// from IObjectFactory
	void registerObjectType(uint64_t objectType, const TObjectFactoryDelegate& setDelegate) override;
	void unregisterObjectType(uint64_t objectType) override;
	void* createObject(uint64_t objectType) override;
};



class TGameApplication :	public IApplication
{
private:	
	std::vector<IScenePtr> scenes;
	std::mutex scenesMutex;
	std::vector<ILogicPtr> logics;
	std::mutex logicsMutex;
	std::unique_ptr<IGameState> state;

	void* createDummyGameState();

protected:
	std::unique_ptr<sf::RenderWindow> window;

	virtual void beforeInitialization();
	virtual void afterInitialization();
	virtual void beforeFinalization();

	virtual sf::RenderWindow* createWindow() = 0;

public:
	std::string appPath;
	TGameFactory factory;

	TGameApplication();
	virtual ~TGameApplication();

	bool initialize(const std::string& filename);
	void finalize();
	virtual void execute();	

	// from IApplication
	IGameState* getState() override;
	void setState(IGameState* state) override;
	void attachScene(IScenePtr scene) override;
	void detachScene(IScenePtr scene) override;
	void addLogic(ILogicPtr logic) override;
	void removeLogic(ILogicPtr logic) override;
};



class TDummyGameState :	public IGameState
{
private:
	IApplication* application;
public:
	TDummyGameState();

	void initialize(IApplication* setApplication) override;
	void finalize() override;
	void update(TGameTime deltaTime) override;
};
