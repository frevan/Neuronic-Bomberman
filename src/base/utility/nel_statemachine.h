#pragma once

#include "../nel_interfaces.h"



namespace nel {

class TStateMachine :	public IStateMachine
{
private:
	IApplication* Application;
	std::unique_ptr<IGameState> CurrentState;
	TGameID NextStateID;

public:
	TStateMachine(IApplication* setApplication);
	~TStateMachine();
		
	void Initialize() override;
	void Finalize() override;
	void SetNextState(TGameID id) override;
	void SwitchToNextState() override;
};

}	// namespace nel