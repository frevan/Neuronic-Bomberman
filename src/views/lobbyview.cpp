#include "lobbyview.h"

#include <iomanip> 
#include <TGUI/TGUI.hpp>

#include "../base/nel_objecttypes.h"

#include "../gameinterfaces.h"
#include "../states/states.h"



TLobbyView::TLobbyView(std::shared_ptr<tgui::Gui> setGUI, nel::IStateMachine* setStateMachine)
:	TTGUIView(setGUI, setStateMachine)
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
	
	// players
	tgui::Label::Ptr playerslbl = std::make_shared<tgui::Label>();
	AddWidgetToGUI(playerslbl);
	playerslbl->setText("Players");
	playerslbl->setPosition(25, 60);
	playerslbl->getRenderer()->setTextColor(sf::Color::White);
	playerslbl->setTextSize(14);
	// --- 
	tgui::Panel::Ptr playerspanel = std::make_shared<tgui::Panel>();
	AddWidgetToGUI(playerspanel);
	playerspanel->setPosition(25, 80);
	playerspanel->setSize(350, 430);
	playerspanel->getRenderer()->setBackgroundColor(sf::Color(40,0,0));
}

void TLobbyView::OnBackBtnClick()
{
	LeaveLobby();
}

void TLobbyView::LeaveLobby()
{
	IClient* client = (IClient*)Application->RetrieveInterface(IID_IClient);
	assert(client);
	client->LeaveLobby();
	client->Disconnect("Leaving");

	StateMachine->SetNextState(SID_Menu);
}