#pragma once

#include <vector>
#include <mutex>

#include "nel_interfaces.h"
#include "nel_objecttypes.h"



namespace nel {



DEFINE_GAMEID(actionToPreviousScreen, "OverlayView::actionToggleFPSLabel")



class TGameState :	public IGameState
{
private:
	std::shared_ptr<ILogic> Logic;
	std::shared_ptr<IEventHandler> EventHandler;
protected:

public:
	IApplication* Application;
	IStateMachine* Owner;
	TGameID StateID;

	TGameState(TGameID setStateID);
	~TGameState() override;

	virtual void Update(TGameTime deltaTime);
	virtual bool ProcessEvent(const sf::Event& event);
	virtual void ProcessInput(nel::TGameID inputID, float value);

	// from IGameState
	void Initialize(IStateMachine* setOwner, IApplication* setApplication) override;
	void Finalize() override;
};



};	// namespace nel