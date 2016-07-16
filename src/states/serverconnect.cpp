#include "serverconnect.h"

#include "states.h"



const int ServerPortConst = 41852;



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
	assert(server->Start(ServerPortConst));

	// connect to local server on next update
	CanConnectToServer = true;
}

void TServerConnectState::Finalize()
{
	IClient* client = (IClient*)Application->RetrieveInterface(IID_IClient);
	assert(client);
	client->Disconnect();

	IServer* server = (IServer*)Application->RetrieveInterface(IID_IServer);
	assert(server);
	server->Stop();

	TGameState::Finalize();
}

void TServerConnectState::Update(nel::TGameTime deltaTime)
{
	TGameState::Update(deltaTime);

	if (CanConnectToServer)
		ConnectToServer();

	IClient* client = (IClient*)Application->RetrieveInterface(IID_IClient);
	assert(client);	
	client->Process();
}

void TServerConnectState::ProcessInput(nel::TGameID inputID, float value)
{
	if (inputID == nel::actionToPreviousScreen && value != 0)
		Owner->SetNextState(SID_Menu);
}

void TServerConnectState::ConnectToServer()
{	
	IClient* client = (IClient*)Application->RetrieveInterface(IID_IClient);
	assert(client);
	client->Connect("127.0.0.1", ServerPortConst);

	CanConnectToServer = false;
}
