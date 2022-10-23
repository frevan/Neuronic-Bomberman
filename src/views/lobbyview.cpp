#include "lobbyview.h"

#include <iomanip>
#include <stdexcept>
#include <TGUI/TGUI.hpp>

#include "../actions.h"

TLobbyView::TLobbyView(TGame* SetGame, tgui::Gui* SetGUI)
	: TTGUIView(SetGame, TViewType::VT_HUMANVIEW, SetGUI),
	NumRoundsEdit(nullptr),
	MapCombo(nullptr),
	Maps(),
	SettingGameData(0),
	SelectedSlot(0)
{
}

TLobbyView::~TLobbyView()
{
}

void TLobbyView::CreateWidgets()
{
	// server name
	tgui::Label::Ptr servernamelbl = std::make_shared<tgui::Label>();
	AddWidgetToGUI(servernamelbl);
	servernamelbl->setText("Name");
	servernamelbl->setPosition(520, 25);
	servernamelbl->getRenderer()->setTextColor(sf::Color::White);
	servernamelbl->setTextSize(14);
	// ---
	GameNameEdit = std::make_shared<tgui::EditBox>();
	AddWidgetToGUI(GameNameEdit);
	GameNameEdit->setText(Game->GameData.GameName);
	GameNameEdit->setPosition(600, 25);
	GameNameEdit->setSize(175, 20);
	GameNameEdit->setTextSize(14);
	GameNameEdit->connect("TextChanged", &TLobbyView::OnGameNameEditTextEntered, this);
	// number of rounds
	tgui::Label::Ptr roundslbl = std::make_shared<tgui::Label>();
	AddWidgetToGUI(roundslbl);
	roundslbl->setText("Rounds");
	roundslbl->setPosition(520, 55);
	roundslbl->getRenderer()->setTextColor(sf::Color::White);
	roundslbl->setTextSize(14);
	// ---
	NumRoundsEdit = std::make_shared<tgui::EditBox>();
	AddWidgetToGUI(NumRoundsEdit);
	NumRoundsEdit->setPosition(600, 55);
	NumRoundsEdit->setSize(175, 20);
	NumRoundsEdit->setInputValidator(tgui::EditBox::Validator::UInt);
	NumRoundsEdit->setText(std::to_string(Game->GameData.MaxRounds));
	NumRoundsEdit->connect("ReturnKeyPressed", &TLobbyView::OnNumRoundsEditReturnKeyPressed, this);
	// map combo
	MapCombo = std::make_shared<tgui::ComboBox>();
	AddWidgetToGUI(MapCombo);
	MapCombo->setPosition(520, 100);
	MapCombo->setSize(255, 25);
	MapCombo->setItemsToDisplay(16);
	FillMapCombo(false);
	MapCombo->setSelectedItemByIndex(0);
	MapCombo->connect("ItemSelected", &TLobbyView::OnMapComboItemSelected, this);

	// back
	tgui::Button::Ptr backbtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(backbtn);
	backbtn->setText("Leave lobby [esc]");
	backbtn->setPosition(25, 535);
	backbtn->setSize(150, 40);
	backbtn->connect("pressed", &TLobbyView::OnBackBtnClick, this);

	// start
	if (Game->IsServer)
	{
		tgui::Button::Ptr startbtn = std::make_shared<tgui::Button>();
		AddWidgetToGUI(startbtn);
		startbtn->setText("Start!");
		startbtn->setPosition(577, 535);
		startbtn->setSize(150, 40);
		startbtn->connect("pressed", &TLobbyView::OnStartBtnClick, this);
	}
	
	// add player
	tgui::Button::Ptr addPlayerBtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(addPlayerBtn);
	addPlayerBtn->setText("Add [=]");
	addPlayerBtn->setPosition(50, 470);
	addPlayerBtn->setSize(100, 30);
	addPlayerBtn->connect("pressed", &TLobbyView::OnAddPlayerBtnClick, this);
	// remove player
	tgui::Button::Ptr removePlayerBtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(removePlayerBtn);
	removePlayerBtn->setText("Remove [-]");
	removePlayerBtn->setPosition(160, 470);
	removePlayerBtn->setSize(100, 30);
	removePlayerBtn->connect("pressed", &TLobbyView::OnRemovePlayerBtnClick, this);
	// remap input controls
	tgui::Button::Ptr remapPlayerControlsBtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(remapPlayerControlsBtn);
	remapPlayerControlsBtn->setText("Configure [0]");
	remapPlayerControlsBtn->setPosition(270, 470);
	remapPlayerControlsBtn->setSize(100, 30);
	remapPlayerControlsBtn->connect("pressed", &TLobbyView::OnRemapPlayerControlsBtnClick, this);
}

void TLobbyView::OnBackBtnClick()
{
	LeaveLobby();
}

void TLobbyView::OnStartBtnClick()
{
	StartMatchNow();
}

void TLobbyView::LeaveLobby()
{
	Game->Client->CloseGame();
}

bool TLobbyView::ProcessInput(TInputID InputID, float Value)
{
	bool handled = false;

	if (Value != 1.0f)
		return false; // only handle key presses

	int mapIndex;

	switch (InputID)
	{
		case actionToPreviousScreen:
			Game->Client->CloseGame();
			Game->SwitchToState(GAMESTATE_MENU);
			handled = true;
			break;

		case actionDoDefaultAction:
			StartMatchNow();
			break;

		case actionLobbyPrevMap:
			if (Game->IsServer)
			{
				mapIndex = MapCombo->getSelectedItemIndex();
				if (mapIndex > 0)
					mapIndex--;
				else
					mapIndex = (int)MapCombo->getItemCount() - 1;
				MapCombo->setSelectedItemByIndex(mapIndex);
				Game->Client->SelectArena(mapIndex);
			}
			break;

		case actionLobbyNextMap:
			if (Game->IsServer)
			{
				mapIndex = MapCombo->getSelectedItemIndex();
				if (mapIndex >= MapCombo->getItemCount() - 1)
					mapIndex = 0;
				else
					mapIndex++;
				MapCombo->setSelectedItemByIndex(mapIndex);
				Game->Client->SelectArena(mapIndex);
			}
			break;

		case actionLobbyPrevSlot:
			SelectedSlot--;
			if (SelectedSlot < 0)
				SelectedSlot = MAX_NUM_SLOTS - 1;
			break;

		case actionLobbyNextSlot:
			SelectedSlot++;
			if (SelectedSlot >= MAX_NUM_SLOTS)
				SelectedSlot = 0;
			break;

		case actionLobbyAddPlayer:
			DoAddPlayer();
			break;

		case actionLobbyRemovePlayer:
			DoRemovePlayer();
			break;

		case actionLobbyRemapPlayerControls:
			DoRemapPlayerControls();
			break;
	};

	return handled;
}

void TLobbyView::StateChanged()
{
	SettingGameData++;

	GameNameEdit->setText(Game->GameData.GameName);

	std::string s = std::to_string(Game->GameData.MaxRounds);
	NumRoundsEdit->setText(s);

	FillMapCombo(true);

	SettingGameData--;
}

void TLobbyView::FillMapCombo(bool KeepSelectedItem)
{
	MapCombo->removeAllItems();

	for (auto it = Game->ArenaNames.begin(); it != Game->ArenaNames.end(); it++)
	{
		std::string caption = (*it);
		MapCombo->addItem(caption);
	}

	int idx = 0;
	if (Game->CurrentArenaIndex >= 0)
		idx = Game->CurrentArenaIndex;

	MapCombo->setSelectedItemByIndex(idx);
}

void TLobbyView::Draw(sf::RenderTarget* target)
{
	TTGUIView::Draw(target);

	const int FIELD_SIZE = 17;
	float MapOffsetX = 520.f;
	float MapOffsetY = 140.f;

	sf::RectangleShape background(sf::Vector2f(495, 600));
	background.setPosition(0, 0);
	background.setFillColor(sf::Color(30, 30, 30));
	background.setOutlineThickness(0.f);
	target->draw(background);

	background.setSize(sf::Vector2f(305, 600));
	background.setPosition(495, 0);
	background.setFillColor(sf::Color(60, 50, 40));
	background.setOutlineThickness(0.f);
	target->draw(background);

	for (size_t row = 0; row < Game->GameData.Arena.Height; row++)
	{
		for (size_t col = 0; col < Game->GameData.Arena.Width; col++)
		{
			sf::Color fieldColor;

			TField field = Game->GameData.Arena.At((uint8_t)col, (uint8_t)row); // read-only access
			switch (field.Type)
			{
				case FIELD_EMPTY: fieldColor = sf::Color::Green; break;
				case FIELD_BRICK: fieldColor = sf::Color::Red; break;
				case FIELD_SOLID:
					fieldColor.r = 128;
					fieldColor.g = 128;
					fieldColor.b = 128;
					break;
			};

			sf::RectangleShape shape(sf::Vector2f(FIELD_SIZE, FIELD_SIZE));
			shape.setPosition(MapOffsetX + col * FIELD_SIZE, MapOffsetY + row * FIELD_SIZE);
			shape.setFillColor(fieldColor);
			shape.setOutlineColor(sf::Color::White);
			shape.setOutlineThickness(1.f);
			target->draw(shape);
		}
	}

	float xpos = 60;
	float ypos = 25;
	sf::Font font = Game->Fonts.ByIndex(Game->Fonts.standard);
	sf::Text t;
	t.setFont(font);
	for (int slot = 0; slot < MAX_NUM_SLOTS; slot++)
	{
		TPlayer* player = &Game->GameData.Players[slot];

		std::string s;

		s = std::to_string(slot + 1) + ". ";
		t.setString(s);
		t.setPosition(sf::Vector2f(xpos, ypos));
		t.setFillColor(Game->GameData.PlayerColors[slot]);
		t.setCharacterSize(18);
		target->draw(t);

		if (player->State == PLAYER_NOTPLAYING)
			s = "---";
		else
			s = player->Name;				
		t.setString(s);
		t.setPosition(sf::Vector2f(xpos + 25, ypos));
		t.setFillColor(sf::Color::White);
		t.setCharacterSize(18);
		target->draw(t);

		s = CreatePlayerShortcutString(slot);
		t.setString(s);
		t.setPosition(sf::Vector2f(xpos + 25, ypos + 24));
		t.setFillColor(sf::Color::White);
		t.setCharacterSize(12);
		target->draw(t);

		if (slot == SelectedSlot)
		{
			sf::CircleShape triangle(7, 3);
			triangle.setRotation(90);
			triangle.setFillColor(sf::Color::White);
			triangle.setPosition(xpos - 10, ypos + 5);
			target->draw(triangle);
		}

		ypos += 40;
	}
}

void TLobbyView::OnMapComboItemSelected(int ItemIndex)
{
	if (SettingGameData)
		return;

	Game->Client->SelectArena(ItemIndex);
}

void TLobbyView::StartMatchNow()
{
	int rounds;
	if (!GetNumRoundsFromEdit(rounds))
		rounds = 1;

	Game->Client->SetNumRounds(rounds);
	Game->Client->SelectArena(MapCombo->getSelectedItemIndex());

	Game->Client->StartMatch();
}

bool TLobbyView::GetNumRoundsFromEdit(int& Value)
{
	bool result = false;

	int rounds = Value;

	std::string s = NumRoundsEdit->getText();
	if (!s.empty())
	{
		try 
		{
			rounds = std::stoi(s);

			if (rounds <= 0)
				rounds = 1;
			if (rounds > MAX_NUM_ROUNDS)
				rounds = MAX_NUM_ROUNDS;

			result = true;
		}
		catch (const std::invalid_argument&)
		{
			result = false;
		}
	}

	if (result)
		Value = rounds;

	return result;
}

void TLobbyView::OnGameNameEditTextEntered(const std::string& Text)
{
	if (SettingGameData == 0)
		Game->Client->SetGameName(Text);
}

void TLobbyView::OnNumRoundsEditReturnKeyPressed(const std::string& Text)
{
	if (SettingGameData == 0)
	{
		int rounds = 1;
		if (GetNumRoundsFromEdit(rounds))
			Game->Client->SetNumRounds(rounds);
	}
}

void TLobbyView::OnAddPlayerBtnClick()
{
	DoAddPlayer();
}

void TLobbyView::OnRemovePlayerBtnClick()
{
	DoRemovePlayer();
}

void TLobbyView::OnRemapPlayerControlsBtnClick()
{
	DoRemapPlayerControls();
}

void TLobbyView::DoAddPlayer()
{
	int slot = -1;
	for (int idx = 0; idx < MAX_NUM_SLOTS; idx++)
	{
		if (Game->GameData.Players[idx].State == PLAYER_NOTPLAYING)
		{
			slot = idx;
			break;
		}
	}

	if (slot >= 0)
	{
		std::string name = "Player " + std::to_string(slot + 1);
		Game->Client->AddPlayer(name, slot);
	}
}

void TLobbyView::DoRemovePlayer()
{
	if (SelectedSlot < 0)
		return;

	Game->Client->RemovePlayer(SelectedSlot);
}

void TLobbyView::DoRemapPlayerControls()
{
}

const std::string TLobbyView::CreatePlayerShortcutString(int Slot)
{
	std::string result = "";

	if (Slot >= 0 && Slot < MAX_NUM_SLOTS)
	{
		for (int i = 0; i < PlayerActionCount; i++)
		{
			int action = actionMatchPlayer1Left + (Slot * PlayerActionCount) + i;

			TInputControl::TPacked control;
			if (!Game->InputMap.GetControlForInput(action, control))
			{
				result = result + " ---";
				continue;
			}

			result = result + " " + ControlToString(control);			
		}
	}

	return result;
}

const std::string TLobbyView::ControlToString(TInputControl::TPacked Control)
{
	std::string result = "?";

	TInputControl::TType type;
	uint8_t controllerIndex, flags;
	uint32_t button;
	TInputControl::Unpack(Control, type, controllerIndex, button, flags);

	if (type == TInputControl::KEYBOARD)
	{
		std::string string = "";
		sf::Keyboard::Key key = static_cast<sf::Keyboard::Key>(button);
		if (key >= sf::Keyboard::A && key <= sf::Keyboard::Z)
		{
			char c = 'A' + button;
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
	else if (type == TInputControl::JOYSTICKAXIS)
	{
		result = "J" + std::to_string(controllerIndex);
		sf::Joystick::Axis axis = static_cast<sf::Joystick::Axis>(button);
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
	else if (type == TInputControl::JOYSTICKBUTTON)
	{
		result = "J" + std::to_string(controllerIndex) + "B" + std::to_string(button);
	}

	std::string modifiers = "";
	if (flags & TInputControl::TFlags::SHIFT)
		modifiers = "shift+";
	if (flags & TInputControl::TFlags::CTRL)
		modifiers += "ctrl+";
	if (flags & TInputControl::TFlags::ALT)
		modifiers = "alt+";

	result = modifiers + result;

	return result;
}