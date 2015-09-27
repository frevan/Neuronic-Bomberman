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
const TGameID INVALID_GAME_ID	= 0;



class IApplication;
class IObjectFactory;
class IGameState;
class IStateMachine;
class IView;
class IActor;
class IComponent;
class IRenderable;
class ILogic;
class IEventHandler;



typedef std::weak_ptr<IView> IViewPtr;
typedef std::weak_ptr<ILogic> ILogicPtr;
typedef std::weak_ptr<IGameState> IGameStatePtr;
typedef std::weak_ptr<IEventHandler> IEventHandlerPtr;



class Interface
{
public:
	virtual ~Interface() {};

	virtual Interface* RetrieveInterface(TGameID id) { return nullptr; };
};



class IApplication :	public Interface
{
public:
	IApplication(): Interface() {};
	virtual ~IApplication() {};

	virtual IObjectFactory& GetFactory() = 0;

	virtual void RequestQuit() = 0;

	virtual void AttachScene(IViewPtr scene) = 0;
	virtual void DetachScene(IViewPtr scene) = 0;

	virtual void AddLogic(ILogicPtr logic) = 0;
	virtual void RemoveLogic(ILogicPtr logic) = 0;	

	virtual void AddEventHandler(IEventHandlerPtr handler) = 0;
	virtual void RemoveEventHandler(IEventHandlerPtr handler) = 0;

	virtual double GetCurrentFps() = 0;
};


// !! delegate functions should follow this signature: 
// void* delegate();
typedef std::function<void*()> IObjectFactoryDelegate;

class IObjectFactory
{
public:
	virtual ~IObjectFactory() {};

	virtual void RegisterObjectType(TObjectType objectType, IObjectFactoryDelegate setDelegate) = 0;
	virtual void UnregisterObjectType(TObjectType objectType) = 0;

	virtual void* CreateObject(TObjectType objectType) = 0;
};



class IGameState :	public Interface
{
public:
	IGameState(): Interface() {};
	virtual ~IGameState() {};

	virtual void Initialize(IStateMachine* setOwner, IApplication* setApplication) = 0;
	virtual void Finalize() = 0;
};



class IStateMachine :	public Interface
{
public:
	IStateMachine(): Interface() {};
	virtual ~IStateMachine() {};

	virtual void Initialize(Interface* setOwner) = 0;
	virtual void Finalize() = 0;
	virtual void SetNextState(TGameID id) = 0;
	virtual void SwitchToNextState() = 0;
};



class IView :	public Interface
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

	IView(): Interface() {};
	virtual ~IView() {};

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



class IRenderable
{
public:
	virtual void Draw(sf::RenderTarget* target) = 0;
};



class ILogic
{
public:
	virtual void Update(TGameTime deltaTime) = 0;
};



class IEventHandler
{
public:
	enum Type {
		APPLICATION = 0,
		STATE,
		OVERLAY,
		REGULAR
	};

	virtual Type GetType() = 0;
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