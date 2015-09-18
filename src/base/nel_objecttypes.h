#pragma once

#include <string>

#include "nel_interfaces.h"



namespace nel {

const TObjectType OT_FPSCalculator = std::hash<std::string>()(std::string("nel::IFPSCalculator"));
const TObjectType OT_StateMachine = std::hash<std::string>()(std::string("nel::IStateMachine"));

};	// namespace nel