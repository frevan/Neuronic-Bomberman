#include "nel_statemachine.h"

#include <cassert>



namespace nel {

TStateMachine::TStateMachine(IApplication* setApplication)
:	IStateMachine(),
	Application(setApplication),
	CurrentState(),
	NextStateID(NO_STATE_CONST),
	Owner(nullptr)
{
}

TStateMachine::~TStateMachine()
{
	assert(!CurrentState);
}

nel::Interface* TStateMachine::RetrieveInterface(nel::TGameID id)
{
	if (Owner)
		return Owner->RetrieveInterface(id);
	else
		return nel::Interface::RetrieveInterface(id);
}

void TStateMachine::Initialize(Interface* setOwner)
{
	Owner = setOwner;
}

void TStateMachine::Finalize()
{
	if (CurrentState)
	{
		CurrentState->Finalize();
		CurrentState.reset();
	}
	Owner = nullptr;
}

void TStateMachine::SetNextState(TGameID id, IGameStateParamsPtr params)
{
	NextStateID = id;
	NextStateParams = params;
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

	CurrentState->Initialize(this, Application, NextStateParams);
	NextStateParams = nullptr;	// dec refcount on shared pointer
}

}	// namespace nel