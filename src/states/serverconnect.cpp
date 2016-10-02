#include "serverconnect.h"

#include "states.h"



const int ServerPortConst = 41852;



TServerConnectState::TServerConnectState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState(SID_Play),
	GUI(setGUI),
	progress(SHOULDCONNECT)
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
	progress = SHOULDCONNECT;
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

	IClient* client = (IClient*)Application->RetrieveInterface(IID_IClient);
	assert(client);
	client->Process();

	switch (progress)
	{
		case SHOULDCONNECT:		
			ConnectToServer();
			progress = CONNECTING;
			break;
		case CONNECTING:	
			if (client->IsConnected()) 
				progress = CONNECTED; 
			break;
		case CONNECTED:
			if (client->IsInLobby())
			{
				progress = INLOBBY;
				Owner->SetNextState(SID_Lobby);
			}
			break;
		case INLOBBY:			
			break;
	};
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
}
