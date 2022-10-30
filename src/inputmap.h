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
		JOYSTICKAXIS,
		JOYSTICKBUTTON
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
	uint32_t Button; // index of button or axis on device
	uint8_t Flags; // flags, modifiers

	TInputControl& operator=(TPacked packed);
	TPacked operator=(const TInputControl& unpacked);
	static TPacked Pack(TType setType, uint8_t setControllerIndex, uint32_t setButton, uint8_t setFlags);
	static void Unpack(TPacked value, TType& type, uint8_t& controllerIndex, uint32_t& button, uint8_t& flags);
	static uint32_t AxisToButtonIndex(sf::Joystick::Axis Axis);
	static sf::Joystick::Axis ButtonIndexToAxis(uint32_t Index);

};

struct TInputBinding
{
	TInputID InputID;
	TInputControl Control;
	TInputControl::TPacked DefaultControl;
	bool Active;
	float Scale;
	float RangeStart;
	float RangeEnd;
	float DefaultValue;
	float Threshold;
	bool Inverted;
};

class TInputMap
{
private:
	std::map<TInputID, TInputBinding> Bindings;
	std::map<TInputID, float> Values;
	bool FindActiveBindingForControl(const TInputControl& control, float Value, TInputBinding& binding);
	bool TranslateEventToControlAndValue(const sf::Event& event, TInputControl& control, float& value);
	void ResetValuesForControl(const TInputControl& control);
	bool TranslateValueForBinding(const TInputBinding& binding, float Value, float& OutValue);
public:
	TInputMap();
	~TInputMap();

	void CheckKeyboardState();
	void CheckJoystickState();
	bool ProcessEvent(const sf::Event& event, TInputID& inputID, float& value);

	// add and remove inputs
	void DefineInput(TInputID setID, TInputControl::TPacked setDefaultControl, float setDefaultValue = 1.0f);
	void DefineInput(TInputID setID, const TInputBinding& SetBinding);
	void ResetInput(TInputID inputID);
	void RemoveInput(TInputID inputID);
	bool GetControlForInput(TInputID inputID, TInputControl::TPacked& result);

	// set input properties
	void SetInputActive(TInputID inputID, bool SetActive);
	void SetInputControl(TInputID inputID, TInputControl::TPacked setControl);
	void SetInputScale(TInputID inputID, float SetScale);
	void SetInputRange(TInputID inputID, float SetStart, float SetEnd);
	void SetInputDefaultValue(TInputID inputID, float SetDefaultValue);
	void SetInputThreshold(TInputID inputID, float SetThreshold);
	void SetInputInverted(TInputID inputID, bool SetInverted);

	float GetValueOfInput(TInputID inputID);
};
