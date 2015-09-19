#include "nel_eventhandler.h"



namespace nel {

TEventHandler::TEventHandler(nel::IEventHandler::Type setType, TEventHandlerProcessFunc setHandler)
:	IEventHandler(), Type(setType), Handler(setHandler) 
{
}

IEventHandler::Type TEventHandler::GetType() 
{ 
	return Type; 
}

bool TEventHandler::ProcessEvent(const sf::Event& event) 
{ 
	return Handler(event); 
}

}	// namespace nel