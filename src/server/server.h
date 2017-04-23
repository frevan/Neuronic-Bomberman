#pragma once

#include <list>
#include <thread>
#include <mutex>
#include <SFML/Network.hpp>

#include "../base/nel_interfaces.h"

#include "servercomms.h"
#include "serverlogic.h"
#include "../gameinterfaces.h"



class TServer :	public IServer,
				public nel::ILogic
{
private:
	TServerComms Communications;
	std::list<TPlayer> Players;	

public:
	nel::IApplication* Application;

	TServer();
	~TServer() override;

	// from IServer
	bool Start(unsigned int port) override;
	void Stop() override;

	// from ILogic
	void Update(nel::TGameTime deltaTime) override;
};