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
typedef uint64_t TObjectType;	// object types for IObjectFactory



const TGameID NO_STATE_CONST	= 0;



class IApplication;
class IObjectFactory;
class IGameState;
class IScene;
class IActor;
class IComponent;
class IRenderable;
class ILogic;
class IEventHandler;



typedef std::weak_ptr<IScene> IScenePtr;
typedef std::weak_ptr<ILogic> ILogicPtr;
typedef std::weak_ptr<IGameState> IGameStatePtr;



class IApplication
{
public:
	virtual ~IApplication() {};

	virtual IObjectFactory& GetFactory() = 0;

	virtual void RequestQuit() = 0;

	virtual void SetNextState(TGameID id) = 0;

	virtual void AttachScene(IScenePtr scene) = 0;
	virtual void DetachScene(IScenePtr scene) = 0;

	virtual void AddLogic(ILogicPtr logic) = 0;
	virtual void RemoveLogic(ILogicPtr logic) = 0;	

	virtual void AddEventHandler(IEventHandler* handler) = 0;
	virtual void RemoveEventHandler(IEventHandler* handler) = 0;

	virtual double GetCurrentFps() = 0;
};


// !! delegate functions should follow this signature: 
// void* delegate();
typedef std::function<void*()> IObjectFactoryDelegate;

class IObjectFactory
{
public:
	virtual void RegisterObjectType(TObjectType objectType, IObjectFactoryDelegate setDelegate) = 0;
	virtual void UnregisterObjectType(TObjectType objectType) = 0;

	virtual void* CreateObject(TObjectType objectType) = 0;
};



class IGameState
{
public:
	virtual ~IGameState() {};

	virtual void Initialize(IApplication* setApplication) = 0;
	virtual void Finalize() = 0;
	virtual bool ProcessEvent(const sf::Event& event) = 0;
	virtual void Update(TGameTime deltaTime) = 0;
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

	virtual void OnAttach(IApplication* setApplication) = 0;
	virtual void OnDetach() = 0;
	virtual void Draw(sf::RenderTarget* target) = 0;
	virtual TViewType GetType() = 0;
	virtual bool IsVisible() = 0;
	virtual void SetVisible(bool newValue) = 0;
};



class IActor
{
public:
	virtual IComponent* GetComponent(int id) = 0;
	virtual void SetComponent(int id, IComponent* value) = 0;
};



class IComponent
{
public:
	virtual std::string GetName() = 0;
	virtual int GetID() = 0;
};



class IRenderable :	public IComponent
{
public:
	virtual void Draw(sf::RenderTarget* target) = 0;
};



class ILogic :	public IComponent
{
public:
	virtual void Update(const TGameTime& deltaTime) = 0;
};



class IEventHandler
{
public:
	virtual bool ProcessEvent(const sf::Event& event) = 0;
};



class IFpsCalculator
{
public:
	double Value;

	virtual ~IFpsCalculator() {};

	virtual void NewFrame() = 0;
};



};	// namespace nel