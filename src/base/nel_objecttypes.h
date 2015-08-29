#pragma once

#include <string>

#include "nel_interfaces.h"



namespace nel {

const TObjectType OT_FPSCalculator = std::hash<std::string>()(std::string("nel::IFPSCalculator"));

};	// namespace nel