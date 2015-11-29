#pragma once

#include "../nel_interfaces.h"



namespace nel {

class TGameStateParams :	public IGameStateParams
{
private:
	std::map<std::string, int> IntValues;
	std::map<std::string, float> FloatValues;
	std::map<std::string, std::string> StringValues;

public:
	TGameStateParams();

	int getInt(const std::string& name) override;
	void setInt(const std::string& name, int value) override;
	float getFloat(const std::string& name) override;
	void setFloat(const std::string& name, float value) override;
	std::string getString(const std::string& name) override;
	void setString(const std::string& name, const std::string& value) override;
};

};