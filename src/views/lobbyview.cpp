#include "lobbyview.h"

#include <iomanip>
#include <stdexcept>
#include <TGUI/TGUI.hpp>

#include "../actions.h"
#include "../utility/stringutil.h"

const float FIRSTSLOT_POS_X = 60.f;
const float FIRSTSLOT_POS_Y = 25.f;
const float SLOT_HEIGHT = 40.f;

TLobbyView::TLobbyView(TGame* SetGame, tgui::Gui* SetGUI)
	: TTGUIView(SetGame, TViewType::VT_HUMANVIEW, SetGUI),
	RenamePlayerEdit(nullptr),
	RenamePlayerApplyBtn(nullptr),
	RenamePlayerCancelBtn(nullptr),
	GameNameEdit(nullptr),
	GameNameLabel(nullptr),
	NumRoundsSpin(nullptr),
	NumRoundsLabel(nullptr),
	MapCombo(nullptr),
	MapNameLabel(nullptr),
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
	if (Game->IsServer)
	{
		GameNameEdit = std::make_shared<tgui::EditBox>();
		AddWidgetToGUI(GameNameEdit);
		GameNameEdit->setText(Game->GameData.GameName);
		GameNameEdit->setPosition(600, 25);
		GameNameEdit->setSize(175, 20);
		GameNameEdit->setTextSize(14);
		GameNameEdit->connect("TextChanged", &TLobbyView::OnGameNameEditTextEntered, this);
	}
	else
	{
		GameNameLabel = std::make_shared<tgui::Label>();
		AddWidgetToGUI(GameNameLabel);
		GameNameLabel->setText(Game->GameData.GameName);
		GameNameLabel->setPosition(600, 25);
		GameNameLabel->getRenderer()->setTextColor(sf::Color::White);
		GameNameLabel->setTextSize(14);
	}
	// number of rounds
	tgui::Label::Ptr roundslbl = std::make_shared<tgui::Label>();
	AddWidgetToGUI(roundslbl);
	roundslbl->setText("Rounds");
	roundslbl->setPosition(520, 55);
	roundslbl->getRenderer()->setTextColor(sf::Color::White);
	roundslbl->setTextSize(14);
	// ---
	NumRoundsLabel = std::make_shared<tgui::Label>();
	AddWidgetToGUI(NumRoundsLabel);
	NumRoundsLabel->setText(std::to_string(Game->GameData.MaxRounds));
	NumRoundsLabel->setPosition(600, 55);
	NumRoundsLabel->getRenderer()->setTextColor(sf::Color::White);
	NumRoundsLabel->setTextSize(14);
	if (Game->IsServer)
	{
		NumRoundsSpin = std::make_shared<tgui::SpinButton>();
		AddWidgetToGUI(NumRoundsSpin);
		NumRoundsSpin->setPosition(675, 55);
		NumRoundsSpin->setSize(50, 20);
		NumRoundsSpin->setMinimum(1.f);
		NumRoundsSpin->setMaximum(MAX_NUM_ROUNDS);
		NumRoundsSpin->setStep(1.f);
		NumRoundsSpin->setValue(Game->GameData.MaxRounds * 1.f);
		NumRoundsSpin->connect("ValueChanged", &TLobbyView::OnNumRoundsSpinChanged, this);
	}
	// map combo
	if (Game->IsServer)
	{
		MapCombo = std::make_shared<tgui::ComboBox>();
		AddWidgetToGUI(MapCombo);
		MapCombo->setPosition(520, 100);
		MapCombo->setSize(255, 25);
		MapCombo->setItemsToDisplay(16);
		MapCombo->connect("ItemSelected", &TLobbyView::OnMapComboItemSelected, this);		
	}
	else
	{
		MapNameLabel = std::make_shared<tgui::Label>();
		AddWidgetToGUI(MapNameLabel);
		MapNameLabel->setPosition(520, 100);
		MapNameLabel->getRenderer()->setTextColor(sf::Color::White);
		MapNameLabel->setTextSize(14);
	}
	FillMapCombo(false);
	if (MapCombo)
		MapCombo->setSelectedItemByIndex(0);

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
	addPlayerBtn->setPosition(30, 470);
	addPlayerBtn->setSize(100, 30);
	addPlayerBtn->connect("pressed", &TLobbyView::OnAddPlayerBtnClick, this);
	// remove player
	tgui::Button::Ptr removePlayerBtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(removePlayerBtn);
	removePlayerBtn->setText("Remove [-]");
	removePlayerBtn->setPosition(140, 470);
	removePlayerBtn->setSize(100, 30);
	removePlayerBtn->connect("pressed", &TLobbyView::OnRemovePlayerBtnClick, this);
	// remap input controls
	tgui::Button::Ptr remapPlayerControlsBtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(remapPlayerControlsBtn);
	remapPlayerControlsBtn->setText("Configure [0]");
	remapPlayerControlsBtn->setPosition(250, 470);
	remapPlayerControlsBtn->setSize(100, 30);
	remapPlayerControlsBtn->connect("pressed", &TLobbyView::OnRemapPlayerControlsBtnClick, this);
	// rename
	tgui::Button::Ptr renamePlayerBtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(renamePlayerBtn);
	renamePlayerBtn->setText("Rename");
	renamePlayerBtn->setPosition(360, 470);
	renamePlayerBtn->setSize(100, 30);
	renamePlayerBtn->connect("pressed", &TLobbyView::OnRenamePlayerBtnClick, this);

	// rename a player - hidden until needed
	// - edit
	RenamePlayerEdit = std::make_shared<tgui::EditBox>();
	AddWidgetToGUI(RenamePlayerEdit);
	RenamePlayerEdit->setVisible(false);
	// - apply btn
	RenamePlayerApplyBtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(RenamePlayerApplyBtn);
	RenamePlayerApplyBtn->setText("OK");
	RenamePlayerApplyBtn->setSize(30, 25);
	RenamePlayerApplyBtn->setVisible(false);
	RenamePlayerApplyBtn->connect("pressed", &TLobbyView::OnRenamePlayerApplyBtnClick, this);
	// - cancel btn
	RenamePlayerCancelBtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(RenamePlayerCancelBtn);
	RenamePlayerCancelBtn->setText("NO");
	RenamePlayerCancelBtn->setSize(30, 25);
	RenamePlayerCancelBtn->setVisible(false);
	RenamePlayerCancelBtn->connect("pressed", &TLobbyView::OnRenamePlayerCancelBtnClick, this);
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
	if (IsSomeEditControlFocused())
		return false;

	if (Value != 1.0f)
		return false; // only handle key presses

	bool handled = false;
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
				SetMapLabelText();
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
				SetMapLabelText();
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

	if (GameNameEdit)
		GameNameEdit->setText(Game->GameData.GameName);
	if (GameNameLabel)
		GameNameLabel->setText(Game->GameData.GameName);

	std::string s = std::to_string(Game->GameData.MaxRounds);
	if (NumRoundsSpin)
		NumRoundsSpin->setValue(Game->GameData.MaxRounds * 1.f);
	if (NumRoundsLabel)
		NumRoundsLabel->setText(s);

	FillMapCombo(true);

	SettingGameData--;
}

void TLobbyView::FillMapCombo(bool KeepSelectedItem)
{
	if (MapCombo)
	{
		MapCombo->removeAllItems();

		for (auto it = Game->ArenaNames.begin(); it != Game->ArenaNames.end(); it++)
		{
			std::string caption = (*it);
			MapCombo->addItem(caption);
		}

		int currentArenaIndex = 0;
		if (Game->CurrentArenaIndex >= 0)
			currentArenaIndex = Game->CurrentArenaIndex;

		MapCombo->setSelectedItemByIndex(currentArenaIndex);
	}

	SetMapLabelText();
}

void TLobbyView::SetMapLabelText()
{
	if (!MapNameLabel)
		return;

	std::string s;
	if (Game->CurrentArenaIndex >= 0 && Game->CurrentArenaIndex < Game->ArenaNames.size())
		s = Game->ArenaNames[Game->CurrentArenaIndex];

	MapNameLabel->setText(s);
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

	float xpos = FIRSTSLOT_POS_X;
	float ypos = FIRSTSLOT_POS_Y;
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

		//s = CreatePlayerShortcutString(slot);
		s = "";
		if (player->State != PLAYER_NOTPLAYING)
		{
			if (player->Owned)
				s = "local";
			else
				s = "remote";
		}
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

		ypos += SLOT_HEIGHT;
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
	int rounds = 1;
	if (NumRoundsSpin)
		rounds = (int)NumRoundsSpin->getValue();

	Game->Client->SetNumRounds(rounds);
	Game->Client->SelectArena(MapCombo->getSelectedItemIndex());

	Game->Client->StartMatch();
}

void TLobbyView::OnGameNameEditTextEntered(const std::string& Text)
{
	if (SettingGameData == 0)
		Game->Client->SetGameName(Text);
}

void TLobbyView::OnNumRoundsSpinChanged(float NewValue)
{
	if (SettingGameData == 0)
	{
		int rounds = (int)NewValue;
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

void TLobbyView::OnRenamePlayerBtnClick()
{
	DoRenamePlayer();
}

void TLobbyView::OnRenamePlayerApplyBtnClick()
{
	DoApplyRenamePlayer();
}

void TLobbyView::OnRenamePlayerCancelBtnClick()
{
	DoCancelRenamePlayer();
}

void TLobbyView::DoAddPlayer()
{
	int slot = -1;

	if (SelectedSlot >= 0)
	{
		if (Game->GameData.Players[SelectedSlot].State == PLAYER_NOTPLAYING)
			slot = SelectedSlot;
	}

	if (slot < 0)
	{
		for (int idx = 0; idx < MAX_NUM_SLOTS; idx++)
		{
			if (Game->GameData.Players[idx].State == PLAYER_NOTPLAYING)
			{
				slot = idx;
				break;
			}
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

void TLobbyView::DoRenamePlayer()
{
	if (RenamePlayerEdit->isVisible())
		return;
	if (SelectedSlot < 0 || SelectedSlot >= MAX_NUM_SLOTS)
		return;

	float xpos = FIRSTSLOT_POS_X + 25;
	float ypos = FIRSTSLOT_POS_Y + (SelectedSlot * SLOT_HEIGHT);

	RenamePlayerEdit->setText(Game->GameData.Players[SelectedSlot].Name);
	RenamePlayerEdit->setPosition(xpos, ypos);
	RenamePlayerEdit->setSize(275, 25);
	RenamePlayerEdit->setVisible(true);

	xpos += RenamePlayerEdit->getSize().x;

	RenamePlayerApplyBtn->setPosition(xpos, ypos);
	RenamePlayerApplyBtn->setVisible(true);

	xpos += RenamePlayerApplyBtn->getSize().x;

	RenamePlayerCancelBtn->setPosition(xpos, ypos);
	RenamePlayerCancelBtn->setVisible(true);

	RenamePlayerEdit->setFocused(true);
}

void TLobbyView::DoApplyRenamePlayer()
{
	std::string s = RenamePlayerEdit->getText();
	trim(s);

	if (!s.empty() && s.size() < 20)
	{
		Game->Client->SetPlayerName(SelectedSlot, s);
		Game->ChosenPlayerName = s;
	}

	RenamePlayerEdit->setVisible(false);
	RenamePlayerApplyBtn->setVisible(false);
	RenamePlayerCancelBtn->setVisible(false);
}

void TLobbyView::DoCancelRenamePlayer()
{
	RenamePlayerEdit->setVisible(false);
	RenamePlayerApplyBtn->setVisible(false);
	RenamePlayerCancelBtn->setVisible(false);
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

bool TLobbyView::IsSomeEditControlFocused()
{
	bool result = false;

	if (GameNameEdit)
		result |= GameNameEdit->isFocused();

	if (RenamePlayerEdit)
		result |= (RenamePlayerEdit->isVisible() && RenamePlayerEdit->isFocused());

	return result;
}