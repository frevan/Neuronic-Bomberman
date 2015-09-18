#include "nel_statemachine.h"

#include <cassert>



namespace nel {

TStateMachine::TStateMachine(IApplication* setApplication)
:	Application(setApplication),
	CurrentState(),
	NextStateID(NO_STATE_CONST)
{
}

TStateMachine::~TStateMachine()
{
	assert(!CurrentState);
}

void TStateMachine::Initialize()
{
}

void TStateMachine::Finalize()
{
	if (CurrentState)
	{
		CurrentState->Finalize();
		CurrentState.reset();
	}
}

void TStateMachine::SetNextState(TGameID id)
{
	NextStateID = id;
}

void TStateMachine::SwitchToNextState()
{
	TGameID newid = NextStateID;
	NextStateID = NO_STATE_CONST;

	if (newid == NO_STATE_CONST)
		return;

	if (CurrentState)
	{
		CurrentState->Finalize();
		CurrentState.reset();
	}

	IGameState* newstate = (IGameState*)Application->GetFactory().CreateObject(newid);
	CurrentState.reset(newstate);
	assert(CurrentState);

	CurrentState->Initialize(this, Application);
}

}	// namespace nel