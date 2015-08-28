#pragma once

#include <vector>
#include <mutex>

#include "nel_interfaces.h"



namespace nel {



class TGameState :	public IGameState
{
public:
	IApplication* Application;

	TGameState();
	~TGameState() override;

	// from IGameState
	void initialize(IApplication* setApplication) override;
	void finalize() override;
	bool processEvent(const sf::Event& event) override;
	void update(TGameTime deltaTime) override;
};



};	// namespace nel