#pragma once

#include <string>
#include <memory>
#include <cstdint>

#include <SFML/Graphics.hpp>



typedef uint32_t TGameTime;



class IApplication;
class IGameState;
class IScene;
class IActor;
class IComponent;
class IRenderable;
class ILogic;
class IInputHandler;



typedef std::shared_ptr<IScene> IScenePtr;
typedef std::shared_ptr<ILogic> ILogicPtr;



class IApplication
{
public:
	virtual ~IApplication() {};

	virtual IGameState* getState() = 0;
	virtual void setState(IGameState* state) = 0;

	virtual void attachScene(IScenePtr scene) = 0;
	virtual void detachScene(IScenePtr scene) = 0;
	virtual void addLogic(ILogicPtr logic) = 0;
	virtual void removeLogic(ILogicPtr logic) = 0;
};


// --- IObjectFactory ---

// !! delegate functions should follow this signature: 
// void Delegate(void** object);
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

	virtual void onAttach() = 0;
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



class IInputHandler :	public IComponent
{
public:
};
