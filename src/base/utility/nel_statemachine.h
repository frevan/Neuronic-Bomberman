#pragma once

#include "../nel_interfaces.h"



namespace nel {

class TStateMachine :	public IStateMachine
{
private:
	IApplication* Application;
	std::unique_ptr<IGameState> CurrentState;
	TGameID NextStateID;
	Interface* Owner;

public:
	TStateMachine(IApplication* setApplication);
	~TStateMachine();

	// from Interface
	nel::Interface* RetrieveInterface(nel::TGameID id) override;
		
	// from IStateMachine
	void Initialize(Interface* setOwner) override;
	void Finalize() override;
	void SetNextState(TGameID id) override;
	void SwitchToNextState() override;
};

}	// namespace nel