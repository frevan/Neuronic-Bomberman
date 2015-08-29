#pragma once

#include <vector>
#include <mutex>

#include "nel_interfaces.h"



namespace nel {



class TGameState :	public IGameState
{
public:
	IApplication* Application;
	TGameID StateID;

	TGameState(TGameID setStateID);
	~TGameState() override;

	// from IGameState
	void Initialize(IApplication* setApplication) override;
	void Finalize() override;
	bool ProcessEvent(const sf::Event& event) override;
	void Update(TGameTime deltaTime) override;
};



};	// namespace nel