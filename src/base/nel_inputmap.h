#pragma once

#include <vector>
#include <map>
#include <functional>

#include "nel_interfaces.h"



namespace nel {



// !! delegate functions should follow this signature: 
// void delegate(TGameID inputID, float value);
typedef std::function<void(TGameID, float)> TInputEventHandler;



struct TInputControl
{
	enum TType
	{
		KEYBOARD = 0,
		MOUSE,
		JOYSTICK
	};
	
	TType type;
	uint8_t index;
	uint16_t button;

	TInputControl& operator=(uint32_t packed) 
	{
		type = TType((packed & 0xFF000000) >> 24);
		index = (packed & 0x00FF0000) >> 16;
		button = packed & 0x0000FFFF;
		return *this;
	};

	uint32_t operator=(const TInputControl& unpacked) 
	{
		return (type << 24) | (index << 16) | button;
	};
};



struct TInputMapping
{
	TInputControl control;
	TGameID id;
	float scale;
};



class TInputMapper
{
private:
	std::map<uint32_t, TInputMapping> mappings;
	std::map<TGameID, TInputEventHandler> eventHandlers;
	TGameID findGameIDForControl(const TInputControl& control);
public:
	TInputMapper();
	~TInputMapper();

	bool processEvent(const sf::Event& event);	

	void defineInput(TGameID setID);
	void bindControlToInput(TGameID inputID, const TInputControl& setControl, float setScale = 1.0f); 
	void unbindControl(TGameID inputID, const TInputControl& control);

	float getValueOfInput(TGameID id);

	void addInputHandler(TGameID inputID, const TInputEventHandler& setHandler);
	void removeInputHandler(TGameID inputID, const TInputEventHandler& setHandler);
};



};	// namespace nel