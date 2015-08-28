#include "nel_inputmap.h"

#include "nel_keyboard.h"



namespace nel {

TInputMapper::TInputMapper()
{
}

TInputMapper::~TInputMapper()
{
}

bool TInputMapper::ProcessEvent(const sf::Event& event)
{
	return false;
}

void TInputMapper::DefineInput(TGameID setID)
{
}

void TInputMapper::BindControlToInput(TGameID inputID, const TInputControl& setControl, float setScale)
{
}

void TInputMapper::UnbindControl(TGameID inputID, const TInputControl& control)
{
}

float TInputMapper::GetValueOfInput(TGameID id)
{
	return 0;
}

void TInputMapper::AddInputHandler(TGameID inputID, const TInputEventHandler& setHandler)
{
}

void TInputMapper::RemoveInputHandler(TGameID inputID, const TInputEventHandler& setHandler)
{
}

TGameID TInputMapper::FindGameIDForControl(const TInputControl& control)
{
	return 0;
}

};	// namespace nel