#include "server.h"

#include <assert.h>



// --- TServer ---

TServer::TServer()
:	IServer(),
	ILogic(),
	Communications(),
	Game(),
	Application(nullptr)
{
}

TServer::~TServer()
{
	Stop();
}

bool TServer::Start(unsigned int port)
{
	Stop();	// just in case

	bool result = Communications.Start(port, &Game);

	return result;
}

void TServer::Stop()
{
	Communications.Stop();
}

void TServer::Update(nel::TGameTime deltaTime)
{
	Communications.Update(deltaTime);
}
