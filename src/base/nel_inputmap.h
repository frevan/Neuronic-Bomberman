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
	
	TType Type;
	uint8_t Index;
	uint16_t Button;

	TInputControl& operator=(uint32_t packed) 
	{
		Type = TType((packed & 0xFF000000) >> 24);
		Index = (packed & 0x00FF0000) >> 16;
		Button = packed & 0x0000FFFF;
		return *this;
	};

	uint32_t operator=(const TInputControl& unpacked) 
	{
		return (Type << 24) | (Index << 16) | Button;
	};
};



struct TInputMapping
{
	TInputControl Control;
	TGameID ID;
	float Scale;
};



class TInputMapper
{
private:
	std::map<uint32_t, TInputMapping> Mappings;
	std::map<TGameID, TInputEventHandler> EventHandlers;
	TGameID FindGameIDForControl(const TInputControl& control);
public:
	TInputMapper();
	~TInputMapper();

	bool ProcessEvent(const sf::Event& event);	

	void DefineInput(TGameID setID);
	void BindControlToInput(TGameID inputID, const TInputControl& setControl, float setScale = 1.0f); 
	void UnbindControl(TGameID inputID, const TInputControl& control);

	float GetValueOfInput(TGameID id);

	void AddInputHandler(TGameID inputID, const TInputEventHandler& setHandler);
	void RemoveInputHandler(TGameID inputID, const TInputEventHandler& setHandler);
};



};	// namespace nel