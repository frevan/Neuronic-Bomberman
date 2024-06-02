#include "optionsview.h"

#include <TGUI/TGUI.hpp>

#include "../actions.h"
#include "../comms.h"
#include "../utility/stringutil.h"

TOptionsView::TOptionsView(TGame* SetGame, tgui::Gui* SetGUI)
	: TTGUIView(SetGame, TViewType::VT_HUMANVIEW, SetGUI)
{
}

TOptionsView::~TOptionsView()
{
}

void TOptionsView::CreateWidgets()
{
}

void TOptionsView::Draw(sf::RenderTarget* target)
{
	TTGUIView::Draw(target);

	sf::Font font = Game->Fonts.ByIndex(Game->Fonts.standard);
	sf::Text t;
	t.setFont(font);

	float xpos = 20, x;
	float ypos = 20, y;

	sf::Color valueColor = sf::Color::White;
	sf::Color labelColor(0x90, 0x90, 0x90);

	std::string s;

	for (int idx = 0; idx < NUM_INPUTS; idx++)
	{
		s = "INPUT " + std::to_string(idx + 1);
		t.setString(s);
		t.setPosition(sf::Vector2f(xpos, ypos));
		t.setFillColor(sf::Color::White);
		t.setCharacterSize(16);
		target->draw(t);

		// label: left
		x = xpos;
		y = ypos + 20;
		t.setString("left");
		t.setPosition(sf::Vector2f(x, y));
		t.setFillColor(labelColor);
		t.setCharacterSize(13);
		target->draw(t);
		// value
		x = xpos + 50;
		y = ypos + 20;
		s = ControlToString(Game->Inputs[idx].Left.Control);
		t.setString(s);
		t.setPosition(sf::Vector2f(x, y));
		t.setFillColor(valueColor);
		t.setCharacterSize(13);
		target->draw(t);

		// label: right
		x = xpos;
		y = ypos + 40;
		t.setString("right");
		t.setPosition(sf::Vector2f(x, y));
		t.setFillColor(labelColor);
		t.setCharacterSize(13);
		target->draw(t);
		// value
		x = xpos + 50;
		y = ypos + 40;
		s = ControlToString(Game->Inputs[idx].Right.Control);
		t.setString(s);
		t.setPosition(sf::Vector2f(x, y));
		t.setFillColor(valueColor);
		t.setCharacterSize(13);
		target->draw(t);

		// label: up
		x = xpos;
		y = ypos + 60;
		t.setString("up");
		t.setPosition(sf::Vector2f(x, y));
		t.setFillColor(labelColor);
		t.setCharacterSize(13);
		target->draw(t);
		// value
		x = xpos + 50;
		y = ypos + 60;
		s = ControlToString(Game->Inputs[idx].Up.Control);
		t.setString(s);
		t.setPosition(sf::Vector2f(x, y));
		t.setFillColor(valueColor);
		t.setCharacterSize(13);
		target->draw(t);

		// label: down
		x = xpos;
		y = ypos + 80;
		t.setString("down");
		t.setPosition(sf::Vector2f(x, y));
		t.setFillColor(labelColor);
		t.setCharacterSize(13);
		target->draw(t);
		// value
		x = xpos + 50;
		y = ypos + 80;
		s = "";
		s = ControlToString(Game->Inputs[idx].Down.Control);
		t.setString(s);
		t.setPosition(sf::Vector2f(x, y));
		t.setFillColor(valueColor);
		t.setCharacterSize(13);
		target->draw(t);

		// label: drop
		x = xpos;
		y = ypos + 100;
		t.setString("drop");
		t.setPosition(sf::Vector2f(x, y));
		t.setFillColor(labelColor);
		t.setCharacterSize(13);
		target->draw(t);
		// value
		x = xpos + 50;
		y = ypos + 100;
		s = ControlToString(Game->Inputs[idx].DropBomb.Control);
		t.setString(s);
		t.setPosition(sf::Vector2f(x, y));
		t.setFillColor(valueColor);
		t.setCharacterSize(13);
		target->draw(t);

		ypos += 140;
	}
}

bool TOptionsView::ProcessInput(TInputID InputID, float Value)
{
	// only handle key presses
	if (!IsInputPressed(Value))
		return false;

	bool handled = true;
	switch (InputID)
	{
		case actionToPreviousScreen:
			Game->SwitchToState(GAMESTATE_MENU);
			break;

		default:
			handled = false;
			break;
	};

	return handled;
}

void TOptionsView::StateChanged()
{
}

const std::string TOptionsView::ControlToString(const TInputControl& Control)
{
	std::string result = "?";

	if (Control.Type == TInputControl::KEYBOARD)
	{
		std::string string = "";
		sf::Keyboard::Key key = static_cast<sf::Keyboard::Key>(Control.Button);
		if (key >= sf::Keyboard::A && key <= sf::Keyboard::Z)
		{
			char c = 'A' + Control.Button;
			string = c;
		}
		else
		{
			switch (key)
			{
			case sf::Keyboard::Num0: string = "0"; break;
			case sf::Keyboard::Num1: string = "1"; break;
			case sf::Keyboard::Num2: string = "2"; break;
			case sf::Keyboard::Num3: string = "3"; break;
			case sf::Keyboard::Num4: string = "4"; break;
			case sf::Keyboard::Num5: string = "5"; break;
			case sf::Keyboard::Num6: string = "6"; break;
			case sf::Keyboard::Num7: string = "7"; break;
			case sf::Keyboard::Num8: string = "8"; break;
			case sf::Keyboard::Num9: string = "9"; break;
			case sf::Keyboard::Escape: string = "esc"; break;
			case sf::Keyboard::LControl: string = "l.ctrl"; break;
			case sf::Keyboard::LShift: string = "l.shift"; break;
			case sf::Keyboard::LAlt: string = "l.alt"; break;
			case sf::Keyboard::LSystem: string = "l.sys"; break;
			case sf::Keyboard::RControl: string = "r.ctrl"; break;
			case sf::Keyboard::RShift: string = "r.shift"; break;
			case sf::Keyboard::RAlt: string = "r.alt"; break;
			case sf::Keyboard::RSystem: string = "r.sys"; break;
			case sf::Keyboard::Menu: string = "menu"; break;
			case sf::Keyboard::LBracket: string = "["; break;
			case sf::Keyboard::RBracket: string = "]"; break;
			case sf::Keyboard::Semicolon: string = ";"; break;
			case sf::Keyboard::Comma: string = ","; break;
			case sf::Keyboard::Period: string = "."; break;
			case sf::Keyboard::Quote: string = "\""; break;
			case sf::Keyboard::Slash: string = "/"; break;
			case sf::Keyboard::Backslash: string = "\\"; break;
			case sf::Keyboard::Tilde: string = "~"; break;
			case sf::Keyboard::Equal: string = "="; break;
			case sf::Keyboard::Hyphen: string = "-"; break;
			case sf::Keyboard::Space: string = "space"; break;
			case sf::Keyboard::Enter: string = "enter"; break;
			case sf::Keyboard::Backspace: string = "backspace"; break;
			case sf::Keyboard::Tab: string = "tab"; break;
			case sf::Keyboard::PageUp: string = "pgup"; break;
			case sf::Keyboard::PageDown: string = "pgdown"; break;
			case sf::Keyboard::End: string = "end"; break;
			case sf::Keyboard::Home: string = "home"; break;
			case sf::Keyboard::Insert: string = "ins"; break;
			case sf::Keyboard::Delete: string = "del"; break;
			case sf::Keyboard::Add: string = "plus"; break;
			case sf::Keyboard::Subtract: string = "min"; break;
			case sf::Keyboard::Multiply: string = "mul"; break;
			case sf::Keyboard::Divide: string = "div"; break;
			case sf::Keyboard::Left: string = "left"; break;
			case sf::Keyboard::Right: string = "right"; break;
			case sf::Keyboard::Up: string = "up"; break;
			case sf::Keyboard::Down: string = "down"; break;
			case sf::Keyboard::Numpad0: string = "num0"; break;
			case sf::Keyboard::Numpad1: string = "num1"; break;
			case sf::Keyboard::Numpad2: string = "num2"; break;
			case sf::Keyboard::Numpad3: string = "num3"; break;
			case sf::Keyboard::Numpad4: string = "num4"; break;
			case sf::Keyboard::Numpad5: string = "num5"; break;
			case sf::Keyboard::Numpad6: string = "num6"; break;
			case sf::Keyboard::Numpad7: string = "num7"; break;
			case sf::Keyboard::Numpad8: string = "num8"; break;
			case sf::Keyboard::Numpad9: string = "num9"; break;
			case sf::Keyboard::F1: string = "F1"; break;
			case sf::Keyboard::F2: string = "F2"; break;
			case sf::Keyboard::F3: string = "F3"; break;
			case sf::Keyboard::F4: string = "F4"; break;
			case sf::Keyboard::F5: string = "F5"; break;
			case sf::Keyboard::F6: string = "F6"; break;
			case sf::Keyboard::F7: string = "F7"; break;
			case sf::Keyboard::F8: string = "F8"; break;
			case sf::Keyboard::F9: string = "F9"; break;
			case sf::Keyboard::F10: string = "F10"; break;
			case sf::Keyboard::F11: string = "F11"; break;
			case sf::Keyboard::F12: string = "F12"; break;
			case sf::Keyboard::F13: string = "F13"; break;
			case sf::Keyboard::F14: string = "F14"; break;
			case sf::Keyboard::F15: string = "F15"; break;
			case sf::Keyboard::Pause: string = "pause"; break;
			}
		}

		result = string;
	}
	else if (Control.Type == TInputControl::JOYSTICKAXIS)
	{
		result = "J" + std::to_string(Control.ControllerIndex);
		sf::Joystick::Axis axis = static_cast<sf::Joystick::Axis>(Control.Button);
		switch (axis)
		{
		case sf::Joystick::R: result += "R"; break;
		case sf::Joystick::U: result += "U"; break;
		case sf::Joystick::V: result += "V"; break;
		case sf::Joystick::X: result += "X"; break;
		case sf::Joystick::Y: result += "Y"; break;
		case sf::Joystick::Z: result += "Z"; break;
		case sf::Joystick::PovX: result += "PovX"; break;
		case sf::Joystick::PovY: result += "PovY"; break;
		}
	}
	else if (Control.Type == TInputControl::JOYSTICKBUTTON)
	{
		result = "J" + std::to_string(Control.ControllerIndex) + "B" + std::to_string(Control.Button);
	}

	std::string modifiers = "";
	if (Control.Flags & TInputControl::TFlags::SHIFT)
		modifiers = "shift+";
	if (Control.Flags & TInputControl::TFlags::CTRL)
		modifiers += "ctrl+";
	if (Control.Flags & TInputControl::TFlags::ALT)
		modifiers = "alt+";

	result = modifiers + result;

	return result;
}