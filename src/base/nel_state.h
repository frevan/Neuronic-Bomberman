#pragma once

#include <vector>
#include <mutex>

#include "nel_interfaces.h"



namespace nel {



class TGameState :	public IGameState,
					public IEventHandler
{
private:
	ILogicPtr Logic;
public:
	IApplication* Application;
	IStateMachine* Owner;
	TGameID StateID;

	TGameState(TGameID setStateID);
	~TGameState() override;

	virtual void Update(TGameTime deltaTime);

	// from IGameState
	void Initialize(IStateMachine* setOwner, IApplication* setApplication) override;
	void Finalize() override;

	// from IEventHandler
	bool ProcessEvent(const sf::Event& event) override;
};



};	// namespace nel