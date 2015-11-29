#include "nel_stateparams.h"



namespace nel {

TGameStateParams::TGameStateParams()
:	IGameStateParams(),
	IntValues(),
	FloatValues(),
	StringValues()
{
}

int TGameStateParams::getInt(const std::string& name)
{
	return IntValues[name];
}

void TGameStateParams::setInt(const std::string& name, int value)
{
	IntValues[name] = value;
}

float TGameStateParams::getFloat(const std::string& name)
{
	return FloatValues[name];
}

void TGameStateParams::setFloat(const std::string& name, float value)
{
	FloatValues[name] = value;
}

std::string TGameStateParams::getString(const std::string& name)
{
	return StringValues[name];
}

void TGameStateParams::setString(const std::string& name, const std::string& value)
{
	StringValues[name] = value;
}

};	// namespace nel