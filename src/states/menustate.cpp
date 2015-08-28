#include "menustate.h"



TMenuState::TMenuState()
:	TGameState(),
	scene(nullptr)
{
	scene.reset(new TMenuScene());
}

TMenuState::~TMenuState()
{
	scene.reset();
}

void TMenuState::initialize(nel::IApplication* setApplication)
{
	TGameState::initialize(setApplication);

	Application->attachScene(scene);
}

void TMenuState::finalize()
{
	Application->detachScene(scene);
}

bool TMenuState::processEvent(const sf::Event& event)
{
	return TGameState::processEvent(event);
}

void TMenuState::update(nel::TGameTime deltaTime)
{
	TGameState::update(deltaTime);
}