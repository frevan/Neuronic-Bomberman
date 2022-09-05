#include "lobbyview.h"

#include <iomanip> 
#include <TGUI/TGUI.hpp>

#include "../actions.h"

TLobbyView::TLobbyView(TGame* SetGame, tgui::Gui* SetGUI)
	: TTGUIView(SetGame, TViewType::VT_HUMANVIEW, SetGUI),
	PlayersListView(nullptr),
	NumRoundsEdit(nullptr),
	MapCombo(nullptr),
	Maps(),
	SettingGameName(0)
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
	servernamelbl->setText("Game name");
	servernamelbl->setPosition(25, 25);
	servernamelbl->getRenderer()->setTextColor(sf::Color::White);
	servernamelbl->setTextSize(14);
	// ---
	GameNameEdit = std::make_shared<tgui::EditBox>();
	AddWidgetToGUI(GameNameEdit);
	GameNameEdit->setText(Game->GameData.GameName);
	GameNameEdit->setPosition(120, 21);
	GameNameEdit->setSize(180, 20);
	GameNameEdit->setTextSize(14);
	GameNameEdit->connect("TextChanged", &TLobbyView::OnGameNameEditTextEntered, this);

	// back
	tgui::Button::Ptr backbtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(backbtn);
	backbtn->setText("Leave lobby");
	backbtn->setPosition(25, 535);
	backbtn->setSize(100, 40);
	backbtn->connect("pressed", &TLobbyView::OnBackBtnClick, this);

	// start
	tgui::Button::Ptr startbtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(startbtn);
	startbtn->setText("Start!");
	startbtn->setPosition(675, 535);
	startbtn->setSize(100, 40);
	startbtn->connect("pressed", &TLobbyView::OnStartBtnClick, this);
	
	// players
	tgui::Label::Ptr playerslbl = std::make_shared<tgui::Label>();
	AddWidgetToGUI(playerslbl);
	playerslbl->setText("Players");
	playerslbl->setPosition(25, 60);
	playerslbl->getRenderer()->setTextColor(sf::Color::White);
	playerslbl->setTextSize(14);
	// ---
	PlayersListView = std::make_shared<tgui::ListView>();
	AddWidgetToGUI(PlayersListView);
	PlayersListView->setPosition(25, 80);
	PlayersListView->setSize(350, 375);
	PlayersListView->getRenderer()->setBackgroundColor(sf::Color(70, 0, 0));
	PlayersListView->getRenderer()->setTextColor(sf::Color::White);
	// ---
	tgui::Button::Ptr addPlayerBtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(addPlayerBtn);
	addPlayerBtn->setText("Add");
	addPlayerBtn->setPosition(75, 470);
	addPlayerBtn->setSize(100, 30);
	addPlayerBtn->connect("pressed", &TLobbyView::OnAddPlayerBtnClick, this);
	// ---
	tgui::Button::Ptr removePlayerBtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(removePlayerBtn);
	removePlayerBtn->setText("Remove");
	removePlayerBtn->setPosition(200, 470);
	removePlayerBtn->setSize(100, 30);
	removePlayerBtn->connect("pressed", &TLobbyView::OnRemovePlayerBtnClick, this);

	// map combo and <> buttons
	MapCombo = std::make_shared<tgui::ComboBox>();
	AddWidgetToGUI(MapCombo);
	MapCombo->setPosition(450, 85);
	MapCombo->setSize(300, 25);
	MapCombo->setItemsToDisplay(16);
	FillMapCombo();
	MapCombo->setSelectedItemByIndex(0);
	MapCombo->connect("ItemSelected", &TLobbyView::OnMapComboItemSelected, this);
	/*
	button = addButton(window, ArtworkPath + imgButton, cbMapPrevious, 420, 85, 25, 25);
	button->setText("<");
	button = addButton(window, ArtworkPath + imgButton, cbMapNext, 755, 85, 25, 25);
	button->setText(">");
	*/

	// number of rounds
	NumRoundsEdit = std::make_shared<tgui::EditBox>();
	AddWidgetToGUI(NumRoundsEdit);
	NumRoundsEdit->setPosition(450, 450);
	NumRoundsEdit->setSize(180, 20);
	NumRoundsEdit->setInputValidator(tgui::EditBox::Validator::UInt);
	NumRoundsEdit->setText(std::to_string(Game->GameData.MaxRounds));
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

	size_t mapIndex;

	switch (InputID)
	{
		case actionToPreviousScreen:
			Game->SwitchToState(STATE_MENU);
			handled = true;
			break;

		case actionDoDefaultAction:
			StartMatchNow();
			break;

		case actionLobbyPrevMap:
			mapIndex = MapCombo->getSelectedItemIndex();
			if (mapIndex > 0)
				mapIndex--;
			else
				mapIndex = MapCombo->getItemCount() - 1;
			MapCombo->setSelectedItemByIndex(mapIndex);
			break;

		case actionLobbyNextMap:
			mapIndex = MapCombo->getSelectedItemIndex();
			if (mapIndex >= MapCombo->getItemCount() - 1)
				mapIndex = 0;
			else
				mapIndex++;
			MapCombo->setSelectedItemByIndex(mapIndex);
			break;
	};

	return handled;
}

void TLobbyView::StateChanged()
{
	SettingGameName++;
	GameNameEdit->setText(Game->GameData.GameName);
	SettingGameName--;

	PlayersListView->removeAllItems();

	for (int i = 0; i < MAX_NUM_SLOTS; i++)
	{
		if (Game->GameData.Players[i].State == PLAYER_NOTPLAYING)
			PlayersListView->addItem("----");
		else
			PlayersListView->addItem(Game->GameData.Players[i].Name);
	}
}

void TLobbyView::FillMapCombo()
{
	MapCombo->removeAllItems();

	for (auto it = Game->Maps.Maps.begin(); it != Game->Maps.Maps.end(); it++)
	{
		std::string caption = (*it)->Caption;
		MapCombo->addItem(caption);
	}
}

void TLobbyView::Draw(sf::RenderTarget* target)
{
	const int FIELD_SIZE = 20;
	float MapOffsetX = 450.f;
	float MapOffsetY = 140.f;

	for (size_t row = 0; row < Game->GameData.Arena.Height; row++)
	{
		for (size_t col = 0; col < Game->GameData.Arena.Width; col++)
		{
			sf::Color fieldColor;

			TField* field = Game->GameData.Arena.At((uint8_t)col, (uint8_t)row);
			switch (field->Type)
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
}

void TLobbyView::OnMapComboItemSelected(int ItemIndex)
{
	Game->Client->SelectArena(MapCombo->getSelectedItemIndex());
}

void TLobbyView::StartMatchNow()
{
	std::string s = NumRoundsEdit->getText();
	int rounds = std::stoi(s);

	Game->Client->SetNumRounds(rounds);
	Game->Client->SelectArena(MapCombo->getSelectedItemIndex());

	Game->SwitchToState(STATE_MATCH);
}

void TLobbyView::OnGameNameEditTextEntered(const std::string& Text)
{
	if (SettingGameName == 0)
		Game->Client->SetGameName(Text);
}

void TLobbyView::OnAddPlayerBtnClick()
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

void TLobbyView::OnRemovePlayerBtnClick()
{
	int idx = PlayersListView->getSelectedItemIndex();
	if (idx < 0)
		return;

	Game->Client->RemovePlayer(idx);
}