#include "nel_inputmap.h"

#include "../nel_keyboard.h"



namespace nel {

TInputControl& TInputControl::operator=(TPacked packed) 
{
	Unpack(packed, Type, ControllerIndex, Button, Flags);
	return *this;
}

TInputControl::TPacked TInputControl::operator=(const TInputControl& unpacked) 
{
	return Pack(Type, ControllerIndex, Button, Flags);
}

TInputControl::TPacked TInputControl::Pack(TType setType, uint8_t setControllerIndex, uint32_t setButton, uint8_t setFlags)
{
	TInputControl::TPacked v = ((uint64_t)setType << 48) | ((uint64_t)setControllerIndex << 40) | ((uint64_t)setButton << 8) | setFlags;
	return v;
}

void TInputControl::Unpack(TPacked value, TType& type, uint8_t& controllerIndex, uint32_t& button, uint8_t& flags)
{
	type = TType(     (value & 0x00FF000000000000) >> 48);
	controllerIndex = (value & 0x0000FF0000000000) >> 40;
	button =          (value & 0x000000FFFFFFFF00) >> 8;
	flags =	          (value & 0x00000000000000FF);
}



TInputMap::TInputMap()
:	Bindings(),
	Values()
{
}

TInputMap::~TInputMap()
{
}

bool TInputMap::ProcessEvent(const sf::Event& event, TGameID& inputID, float& value)
{
	TInputControl control;
	if (!TranslateEventToControlAndValue(event, control, value))
		return false;

	TInputBinding binding;
	if (FindBindingForControl(control, binding))
	{	
		inputID = binding.InputID;
		value = value * binding.Scale;

		Values[inputID] = value;

		return true;
	}

	return false;
}

void TInputMap::DefineInput(TGameID setID, TInputControl::TPacked setDefaultControl, float setDefaultValue, float setScale)
{
	TInputBinding binding;
	binding.InputID = setID;
	binding.Control = setDefaultControl;
	binding.DefaultControl = setDefaultControl;
	binding.Scale = setScale;

	auto it = Bindings.find(setID);
	if (it == Bindings.end())
		Bindings.insert( std::pair<TGameID,TInputBinding>(setID, binding) );		
	else
		it->second = binding;

	auto it2 = Values.find(setID);
	if (it2 == Values.end())
		Values.insert( std::pair<TGameID, float>(setID, setDefaultValue) );
}

void TInputMap::BindControlToInput(TGameID inputID, TInputControl::TPacked setControl, float setScale)
{
	auto it = Bindings.find(inputID);
	if (it != Bindings.end())
	{
		// maybe this should reassign the entire binding, since it's a struct?
		it->second.Control = setControl;
		it->second.Scale = setScale;
	}
}

void TInputMap::ResetInput(TGameID inputID)
{
	auto it = Bindings.find(inputID);
	if (it != Bindings.end())
		// maybe this should reassign the entire binding, since it's a struct?
		it->second.Control = it->second.DefaultControl;
}

float TInputMap::GetValueOfInput(TGameID inputID)
{
	auto it = Values.find(inputID);
	if (it != Values.end())
		return it->second;

	return 0;
}

TGameID TInputMap::FindInputIDForControl(const TInputControl& control)
{
	TInputBinding binding;
	if (FindBindingForControl(control, binding))
		return binding.InputID;

	return INVALID_GAME_ID;
}

bool TInputMap::FindBindingForControl(const TInputControl& control, TInputBinding& binding)
{
	for (auto it = Bindings.begin(); it != Bindings.end(); it++)
	{		
		if (it->second.Control.Type == control.Type && it->second.Control.ControllerIndex == control.ControllerIndex && it->second.Control.Button == control.Button)
		{
			binding = it->second;
			return true;
		}
	}

	return false;
}

bool TInputMap::TranslateEventToControlAndValue(const sf::Event& event, TInputControl& control, float& value)
{
	bool found = true;

	if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
	{
		control.Type = TInputControl::TType::KEYBOARD;
		control.ControllerIndex = 0;
		control.Button = event.key.code;
		control.Flags = 0;
		if (event.key.alt)		control.Flags |= TInputControl::TFlags::ALT;
		if (event.key.control)	control.Flags |= TInputControl::TFlags::CTRL;
		if (event.key.shift)	control.Flags |= TInputControl::TFlags::SHIFT;

		value = 1.f;
		if (event.type == sf::Event::KeyReleased)
			value = 0.f;
	}
	else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased)
	{
		control.Type = TInputControl::TType::MOUSE;
		control.ControllerIndex = 0;
		control.Button = event.mouseButton.button;
		control.Flags = 0;

		value = 1.f;
		if (event.type == sf::Event::MouseButtonReleased)
			value = 0.f;
	}
	else if (event.type == sf::Event::MouseWheelScrolled)
	{
		///< The mouse wheel was scrolled (data in event.mouseWheelScroll)
		control.Type = TInputControl::TType::MOUSEWHEEL;
		control.ControllerIndex = 0;
		control.Button = event.mouseWheelScroll.wheel;
		control.Flags = 0;

		value = event.mouseWheelScroll.delta;	// NOTE: should probably scale this to -1.0 .. +1.0 ?
	}
	else if (event.type == sf::Event::MouseMoved)
	{
		///< The mouse cursor moved (data in event.mouseMove)
		found = false;
	}
	else if (event.type == sf::Event::JoystickButtonPressed || event.type == sf::Event::JoystickButtonReleased)
	{
		///< A joystick button was pressed (data in event.joystickButton)
		///< A joystick button was released (data in event.joystickButton)
		found = false;
	}
	else if (event.type == sf::Event::JoystickMoved)
	{
		///< The joystick moved along an axis (data in event.joystickMove)
		found = false;
	}
	else if (event.type == sf::Event::TouchBegan || event.type == sf::Event::TouchEnded)
	{
		///< A touch event began (data in event.touch)
		///< A touch event ended (data in event.touch)
		found = false;
	}
	else if (event.type == sf::Event::TouchMoved)
	{        
		///< A touch moved (data in event.touch)
		found = false;
	}
	else if (event.type == sf::Event::SensorChanged)
	{
		///< A sensor value changed (data in event.sensor)
		found = false;
	}
	else
		found = false;

	return found;
}

};	// namespace nel