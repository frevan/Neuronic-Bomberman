#include "nel_inputmap.h"

#include "nel_keyboard.h"



namespace nel {



TInputMapper::TInputMapper()
{
}

TInputMapper::~TInputMapper()
{
}

bool TInputMapper::processEvent(const sf::Event& event)
{
	return false;
}

void TInputMapper::defineInput(TGameID setID)
{
}

void TInputMapper::bindControlToInput(TGameID inputID, const TInputControl& setControl, float setScale)
{
}

void TInputMapper::unbindControl(TGameID inputID, const TInputControl& control)
{
}

float TInputMapper::getValueOfInput(TGameID id)
{
	return 0;
}

void TInputMapper::addInputHandler(TGameID inputID, const TInputEventHandler& setHandler)
{
}

void TInputMapper::removeInputHandler(TGameID inputID, const TInputEventHandler& setHandler)
{
}

TGameID TInputMapper::findGameIDForControl(const TInputControl& control)
{
	return 0;
}



};	// namespace nel