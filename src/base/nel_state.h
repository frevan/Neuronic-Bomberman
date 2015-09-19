#pragma once

#include <vector>
#include <mutex>

#include "nel_interfaces.h"



namespace nel {



class TGameState :	public IGameState
{
private:
	std::shared_ptr<ILogic> Logic;
	std::shared_ptr<IEventHandler> EventHandler;
public:
	IApplication* Application;
	IStateMachine* Owner;
	TGameID StateID;

	TGameState(TGameID setStateID);
	~TGameState() override;

	virtual void Update(TGameTime deltaTime);
	virtual bool ProcessEvent(const sf::Event& event);

	// from IGameState
	void Initialize(IStateMachine* setOwner, IApplication* setApplication) override;
	void Finalize() override;
};



};	// namespace nel