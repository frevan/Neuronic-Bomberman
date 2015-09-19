#pragma once

#include "../nel_interfaces.h"



namespace nel {

// !! delegate functions should follow this signature: 
// bool delegate(const sf::Event& event);
typedef std::function<bool(const sf::Event&)> TEventHandlerProcessFunc;

class TEventHandler :	public nel::IEventHandler
{
private:
	nel::IEventHandler::Type Type;
	TEventHandlerProcessFunc Handler;
public:
	TEventHandler(IEventHandler::Type setType, TEventHandlerProcessFunc setHandler);

	// from IEventHandler
	IEventHandler::Type GetType() override;
	bool ProcessEvent(const sf::Event& event) override;
};

}	// namespace nel