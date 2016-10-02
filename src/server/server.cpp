#include "server.h"

#include <assert.h>



// --- TServer ---

TServer::TServer()
:	IServer(),
	ILogic(),
	Communications()
{
}

TServer::~TServer()
{
	Stop();
}

bool TServer::Start(unsigned int port)
{
	Stop();	// just in case

	bool result = Communications.Start(port);

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