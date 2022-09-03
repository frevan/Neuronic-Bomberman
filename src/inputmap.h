#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <functional>

#include <SFML/Window.hpp>

typedef uint32_t TInputID;

const TInputID INVALID_INPUT_ID = 0;

struct TInputControl
{
	enum TType
	{
		INVALID	= 0,
		KEYBOARD,
		MOUSE,
		MOUSEWHEEL,
		JOYSTICK
	};

	enum TFlags 
	{
		SHIFT		= 1 << 0,
		ALT			= 1 << 1,
		CTRL		= 1 << 2
	};

	typedef uint64_t TPacked;
	
	TType Type; // device type
	uint8_t ControllerIndex; // index of device of this type (joysticks)
	uint32_t Button; // button on device
	uint8_t Flags; // flags, modifiers

	TInputControl& operator=(TPacked packed);
	TPacked operator=(const TInputControl& unpacked);
	static TPacked Pack(TType setType, uint8_t setControllerIndex, uint32_t setButton, uint8_t setFlags);
	static void Unpack(TPacked value, TType& type, uint8_t& controllerIndex, uint32_t& button, uint8_t& flags);

};

struct TInputBinding
{
	TInputID InputID;
	TInputControl Control;
	TInputControl::TPacked DefaultControl;
	float Scale;
};

class TInputMap
{
private:
	std::map<TInputID, TInputBinding> Bindings;
	std::map<TInputID, float> Values;
	TInputID FindInputIDForControl(const TInputControl& control);
	bool FindBindingForControl(const TInputControl& control, TInputBinding& binding);
	bool TranslateEventToControlAndValue(const sf::Event& event, TInputControl& control, float& value);
public:
	TInputMap();
	~TInputMap();

	void CheckKeyboardState();
	bool ProcessEvent(const sf::Event& event, TInputID& inputID, float& value);

	void DefineInput(TInputID setID, TInputControl::TPacked setDefaultControl, float setDefaultValue = 1.0f, float setScale = 1.0f);
	void BindControlToInput(TInputID inputID, TInputControl::TPacked setControl, float setScale = 1.0f);
	void ResetInput(TInputID inputID);
	void RemoveInput(TInputID inputID);

	float GetValueOfInput(TInputID inputID);
};
