#include "inputmap.h"



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
	type = TType((value & 0x00FF000000000000) >> 48);
	controllerIndex = static_cast<uint8_t>((value & 0x0000FF0000000000) >> 40);
	button = static_cast<uint8_t>((value & 0x000000FFFFFFFF00) >> 8);
	flags =	 (value & 0x00000000000000FF);
}

uint32_t TInputControl::AxisToButtonIndex(sf::Joystick::Axis Axis)
{
	return static_cast<uint32_t>(Axis);
}

sf::Joystick::Axis TInputControl::ButtonIndexToAxis(uint32_t Index)
{
	return static_cast<sf::Joystick::Axis>(Index);
}



TInputMap::TInputMap()
:	Bindings(),
	Values()
{
}

TInputMap::~TInputMap()
{
}

bool TInputMap::ProcessEvent(const sf::Event& event, TInputID& inputID, float& value)
{
	float v = 0.f;
	value = 0.f;
	bool result = false;

	TInputControl control;
	if (TranslateEventToControlAndValue(event, control, v))
	{
		TInputBinding binding;
		if (FindBindingForControl(control, v, binding))
		{
			ResetValuesForControl(control);
			if (TranslateValueForBinding(binding, v, value))
			{
				inputID = binding.InputID;
				Values[inputID] = value;
				result = true;
			}
		}
	}

	return result;
}

void TInputMap::DefineInput(TInputID setID, TInputControl::TPacked setDefaultControl, float setDefaultValue)
{
	TInputBinding binding;
	binding.InputID = setID;
	binding.Control = setDefaultControl;
	binding.DefaultControl = setDefaultControl;
	binding.Scale = 1.f;
	binding.RangeStart = 0.f;
	binding.RangeEnd = 1.f;
	binding.DefaultValue = setDefaultValue;
	binding.Threshold = 0.f;
	binding.Inverted = false;

	auto it = Bindings.find(setID);
	if (it == Bindings.end())
		Bindings.insert( std::pair<TInputID,TInputBinding>(setID, binding) );		
	else
		it->second = binding;

	auto it2 = Values.find(setID);
	if (it2 == Values.end())
		Values.insert( std::pair<TInputID, float>(setID, setDefaultValue) );
}

void TInputMap::RemoveInput(TInputID inputID)
{
	Bindings.erase(inputID);
}

void TInputMap::SetInputControl(TInputID inputID, TInputControl::TPacked setControl)
{
	auto it = Bindings.find(inputID);
	if (it != Bindings.end())
		it->second.Control = setControl;
}

void TInputMap::ResetInput(TInputID inputID)
{
	auto it = Bindings.find(inputID);
	if (it != Bindings.end())
		// maybe this should reassign the entire binding, since it's a struct?
		it->second.Control = it->second.DefaultControl;
}

float TInputMap::GetValueOfInput(TInputID inputID)
{
	float result = 0.f;

	auto it = Values.find(inputID);
	if (it != Values.end())
		result = it->second;

	return result;
}

bool TInputMap::FindBindingForControl(const TInputControl& control, float Value, TInputBinding& binding)
{
	bool result = false;
	TInputBinding* b;
	TInputControl* c;

	for (auto it = Bindings.begin(); it != Bindings.end(); it++)
	{		
		b = &it->second;
		c = &it->second.Control;
		if (c->Type == control.Type && c->ControllerIndex == control.ControllerIndex && c->Button == control.Button && c->Flags == control.Flags)
		{
			if (Value >= b->RangeStart && Value <= b->RangeEnd)
			{
				binding = it->second;
				result = true;
				break;
			}
		}
	}

	return result;
}

void TInputMap::ResetValuesForControl(const TInputControl& control)
{
	TInputBinding* b;
	TInputControl* c;

	for (auto it = Bindings.begin(); it != Bindings.end(); it++)
	{
		b = &it->second;
		c = &it->second.Control;
		if (c->Type == control.Type && c->ControllerIndex == control.ControllerIndex && c->Button == control.Button && c->Flags == control.Flags)
			Values[b->InputID] = b->DefaultValue;
	}
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
		control.Type = TInputControl::TType::JOYSTICKBUTTON;
		control.ControllerIndex = event.joystickButton.joystickId;
		control.Button = event.joystickButton.button;
		control.Flags = 0;

		value = 1.f;
		if (event.type == sf::Event::JoystickButtonReleased)
			value = 0.f;
	}
	else if (event.type == sf::Event::JoystickMoved)
	{
		///< The joystick moved along an axis (data in event.joystickMove)
		control.Type = TInputControl::TType::JOYSTICKAXIS;
		control.ControllerIndex = event.joystickMove.joystickId;
		control.Button = TInputControl::AxisToButtonIndex(event.joystickMove.axis);
		control.Flags = 0;

		value = (event.joystickMove.position + 100.f) / 200.f;
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

void TInputMap::CheckKeyboardState()
{
	for (auto it = Bindings.begin(); it != Bindings.end(); it++)
	{
		if ((*it).second.Control.Type == TInputControl::KEYBOARD)
		{
			sf::Keyboard::Key key = (sf::Keyboard::Key)(*it).second.Control.Button;
			bool pressed = sf::Keyboard::isKeyPressed(key);
			if (pressed)
				Values[(*it).first] = 1.0f;
			else
				Values[(*it).first] = 0.0f;
		}
	}
}

void TInputMap::CheckJoystickState()
{
	for (auto it = Bindings.begin(); it != Bindings.end(); it++)
	{
		if ((*it).second.Control.Type == TInputControl::JOYSTICKBUTTON)
		{
			bool pressed = sf::Joystick::isButtonPressed((*it).second.Control.ControllerIndex, (*it).second.Control.Button);
			if (pressed)
				Values[(*it).first] = 1.0f;
			else
				Values[(*it).first] = 0.0f;
		}
		else if ((*it).second.Control.Type == TInputControl::JOYSTICKAXIS)
		{
			float position = sf::Joystick::getAxisPosition((*it).second.Control.ControllerIndex, TInputControl::ButtonIndexToAxis((*it).second.Control.Button));
			position = (position + 100.f) / 200.f;

			float value;
			TranslateValueForBinding((*it).second, position, value);
			Values[(*it).first] = value; 
		}
	}
}

void TInputMap::SetInputScale(TInputID inputID, float SetScale)
{
	auto it = Bindings.find(inputID);
	if (it != Bindings.end())
		it->second.Scale = SetScale;
}

void TInputMap::SetInputRange(TInputID inputID, float SetStart, float SetEnd)
{
	auto it = Bindings.find(inputID);
	if (it != Bindings.end())
	{
		it->second.RangeStart = SetStart;
		it->second.RangeEnd = SetEnd;
	}
}

void TInputMap::SetInputDefaultValue(TInputID inputID, float SetDefaultValue)
{
	auto it = Bindings.find(inputID);
	if (it != Bindings.end())
		it->second.DefaultValue = SetDefaultValue;
}

void TInputMap::SetInputThreshold(TInputID inputID, float SetThreshold)
{
	auto it = Bindings.find(inputID);
	if (it != Bindings.end())
		it->second.Threshold = SetThreshold;
}

void TInputMap::SetInputInverted(TInputID inputID, bool SetInverted)
{
	auto it = Bindings.find(inputID);
	if (it != Bindings.end())
		it->second.Inverted = SetInverted;
}

bool TInputMap::TranslateValueForBinding(const TInputBinding& binding, float Value, float& OutValue)
{
	bool result = false;

	if (Value >= binding.RangeStart || Value <= binding.RangeEnd)
	{
		float v = (Value - binding.RangeStart) * (1.f / (binding.RangeEnd - binding.RangeStart));

		if (binding.Inverted)
			v = 1.f - v;

		if (v >= binding.Threshold)
		{
			OutValue = v * binding.Scale;
			result = true;
		}
	}

	if (!result)
		OutValue = binding.DefaultValue;
	return result;
}