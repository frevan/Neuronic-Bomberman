#include "serverconnect.h"

#include "states.h"



TServerConnectState::TServerConnectState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState(SID_Play),
	GUI(setGUI),
	CanConnectToServer(false)
{	
}

void TServerConnectState::Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication, nel::IGameStateParamsPtr params)
{
	TGameState::Initialize(setOwner, setApplication, params);

	// start local server
	IServer* server = (IServer*)Application->RetrieveInterface(IID_IServer);
	assert(server);
	assert(server->Start(41852));

	// connect to local server on next update
	CanConnectToServer = true;
}

void TServerConnectState::Finalize()
{
	TGameState::Finalize();
}

void TServerConnectState::Update(nel::TGameTime deltaTime)
{
	TGameState::Update(deltaTime);

	if (CanConnectToServer)
		ConnectToServer();
}

void TServerConnectState::ProcessInput(nel::TGameID inputID, float value)
{
	if (inputID == nel::actionToPreviousScreen && value != 0)
		Owner->SetNextState(SID_Menu);
}

void TServerConnectState::ConnectToServer()
{
	TODO: create server communications

}
