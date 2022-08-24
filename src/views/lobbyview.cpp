#include "lobbyview.h"

#include <iomanip> 
#include <TGUI/TGUI.hpp>

#include "../actions.h"

TLobbyView::TLobbyView(TGame* SetGame, tgui::Gui* SetGUI)
:	TTGUIView(SetGame, TViewType::VT_HUMANVIEW, SetGUI),
	PlayersListView(nullptr),
	Maps()
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
	tgui::EditBox::Ptr servernameedit = std::make_shared<tgui::EditBox>();
	AddWidgetToGUI(servernameedit);
	servernameedit->setText("Bombi");
	servernameedit->setPosition(120, 21);
	servernameedit->setSize(180, 20);
	servernameedit->setTextSize(14);

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
	PlayersListView->setSize(350, 430);
	PlayersListView->getRenderer()->setBackgroundColor(sf::Color(70, 0, 0));
	PlayersListView->getRenderer()->setTextColor(sf::Color::White);

	// map combo and <> buttons
	MapCombo = std::make_shared<tgui::ComboBox>();
	AddWidgetToGUI(MapCombo);
	MapCombo->setPosition(450, 85);
	MapCombo->setSize(300, 25);
	MapCombo->setItemsToDisplay(16);
	FillMapCombo();
	MapCombo->setSelectedItemByIndex(0);
	/*
	button = addButton(window, ArtworkPath + imgButton, cbMapPrevious, 420, 85, 25, 25);
	button->setText("<");
	button = addButton(window, ArtworkPath + imgButton, cbMapNext, 755, 85, 25, 25);
	button->setText(">");
	*/
}

void TLobbyView::OnBackBtnClick()
{
	LeaveLobby();
}

void TLobbyView::OnStartBtnClick()
{
	Game->Client->SelectArena(MapCombo->getSelectedItemIndex());
	Game->SwitchToState(STATE_MATCH);
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

	switch (InputID)
	{
		case actionToPreviousScreen:
			Game->SwitchToState(STATE_MENU);
			handled = true;
			break;

		case actionDoDefaultAction:
			Game->Client->SelectArena(MapCombo->getSelectedItemIndex());
			Game->SwitchToState(STATE_MATCH);
			break;
	};

	return handled;
}

void TLobbyView::StateChanged()
{
	PlayersListView->removeAllItems();

	for (int i = 0; i < MAX_NUM_SLOTS; i++)
	{
		if (Game->GameData.Players[i].State == PLAYER_NOTPLAYING)
			continue;

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