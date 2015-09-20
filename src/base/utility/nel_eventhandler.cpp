#include "nel_eventhandler.h"



namespace nel {

TEventHandler::TEventHandler(nel::IEventHandler::Type setType, TProcessEventFunc setProcessEventFunc, TProcessInputProc setProcessInputProc)
:	IEventHandler(), Type(setType), 
	ProcessEventFunc(setProcessEventFunc),
	ProcessInputProc(setProcessInputProc),
	InputMap()
{
}

IEventHandler::Type TEventHandler::GetType() 
{ 
	return Type; 
}

bool TEventHandler::ProcessEvent(const sf::Event& event) 
{ 
	// first try to find an input
	TGameID inputID;
	float value;
	if (InputMap.ProcessEvent(event, inputID, value) && (ProcessInputProc))
	{
		ProcessInputProc(inputID, value);
		return true;
	}

	// handle event if no input was found
	if (ProcessEventFunc)
		return ProcessEventFunc(event); 

	return false;
}

}	// namespace nel