#pragma once

#include "../base/nel_state.h"
#include "../scenes/menuscene.h"



class TMenuState :	public nel::TGameState
{
private:
	std::shared_ptr<TMenuScene> scene;
public:
	TMenuState();
	~TMenuState() override;

	// from TGameState
	void initialize(nel::IApplication* setApplication) override;
	void finalize() override;
	bool processEvent(const sf::Event& event) override;
	void update(nel::TGameTime deltaTime) override;
};