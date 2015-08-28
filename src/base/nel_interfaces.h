#pragma once

#include <string>
#include <memory>
#include <cstdint>
#include <functional>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>



namespace nel {



typedef uint32_t TGameTime;		// in milliseconds
typedef uint64_t TGameID;		// used for any type of unique id



class IApplication;
class IGameState;
class IScene;
class IActor;
class IComponent;
class IRenderable;
class ILogic;
class IEventHandler;



typedef std::shared_ptr<IScene> IScenePtr;
typedef std::shared_ptr<ILogic> ILogicPtr;
typedef std::shared_ptr<IGameState> IGameStatePtr;



class IApplication
{
public:
	virtual ~IApplication() {};

	virtual void requestQuit() = 0;

	virtual void setNextState(IGameState* nextState) = 0;

	virtual void attachScene(IScenePtr scene) = 0;
	virtual void detachScene(IScenePtr scene) = 0;

	virtual void addLogic(ILogicPtr logic) = 0;
	virtual void removeLogic(ILogicPtr logic) = 0;	

	virtual void addEventHandler(IEventHandler* handler) = 0;
	virtual void removeEventHandler(IEventHandler* handler) = 0;
};


// !! delegate functions should follow this signature: 
// void* delegate();
typedef std::function<void*()> TObjectFactoryDelegate;

class IObjectFactory
{
public:
	virtual void registerObjectType(uint64_t objectType, const TObjectFactoryDelegate& setDelegate) = 0;
	virtual void unregisterObjectType(uint64_t objectType) = 0;

	virtual void* createObject(uint64_t objectType) = 0;
};



class IGameState
{
public:
	virtual ~IGameState() {};

	virtual void initialize(IApplication* setApplication) = 0;
	virtual void finalize() = 0;
	virtual bool processEvent(const sf::Event& event) = 0;
	virtual void update(TGameTime deltaTime) = 0;
};



class IScene
{
private:
public:
	typedef enum 
	{
		VT_SYSTEMVIEW	= 0,	// any other view
		VT_HUMANVIEW,			// local human player
		VT_NETWORKVIEW,			// remote human player
		VT_AIVIEW				// (local) AI player
	} TViewType;

	virtual ~IScene() {};

	virtual void onAttach(IApplication* setApplication) = 0;
	virtual void onDetach() = 0;

	virtual void draw(sf::RenderTarget* target) = 0;

	virtual TViewType getType() = 0;
};



class IActor
{
public:
	virtual IComponent* getComponent(int id) = 0;
	virtual void setComponent(int id, IComponent* value) = 0;
};



class IComponent
{
public:
	virtual std::string getName() = 0;
	virtual int getID() = 0;
};



class IRenderable :	public IComponent
{
public:
	virtual void draw(sf::RenderTarget* target) = 0;
};



class ILogic :	public IComponent
{
public:
	virtual void update(const TGameTime& deltaTime) = 0;
};



class IEventHandler
{
public:
	virtual bool processEvent(const sf::Event& event) = 0;
};



};	// namespace nel