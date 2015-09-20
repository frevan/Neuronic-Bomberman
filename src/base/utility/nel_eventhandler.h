#pragma once

#include "../nel_interfaces.h"
#include "nel_inputmap.h"



namespace nel {

// !! delegate functions should follow this signature: 
// bool delegate(const sf::Event& event);
typedef std::function<bool(const sf::Event&)> TProcessEventFunc;
typedef std::function<void(TGameID inputID, float value)> TProcessInputProc;

class TEventHandler :	public nel::IEventHandler
{
private:
	IEventHandler::Type Type;
	TProcessEventFunc ProcessEventFunc;
	TProcessInputProc ProcessInputProc;
public:
	TInputMap InputMap;

	TEventHandler(IEventHandler::Type setType, TProcessEventFunc setProcessEventFunc, TProcessInputProc setProcessInputProc);

	// from IEventHandler
	IEventHandler::Type GetType() override;
	bool ProcessEvent(const sf::Event& event) override;
};

}	// namespace nel