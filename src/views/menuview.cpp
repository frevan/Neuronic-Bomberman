#include "menuview.h"

#include <iomanip> 
#include <TGUI/TGUI.hpp>

#include "../states/lobbystate.h"
#include "../actions.h"

TMenuView::TMenuView(TGame* SetGame, tgui::Gui* SetGUI)
:	TTGUIView(SetGame, TViewType::VT_HUMANVIEW, SetGUI)
{
}

TMenuView::~TMenuView()
{
}

void TMenuView::CreateWidgets()
{
	// game name label
	tgui::Label::Ptr namelbl = std::make_shared<tgui::Label>();
	AddWidgetToGUI(namelbl);
	namelbl->setText("Neuronic Bomberman");
	namelbl->setPosition(25, 25);
	namelbl->getRenderer()->setTextColor(sf::Color::Red);
	namelbl->setTextSize(35);

	// new game
	tgui::Button::Ptr newgamebtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(newgamebtn);
	newgamebtn->setText("New game");
	newgamebtn->setPosition(300, 200);
	newgamebtn->setSize(200, 60);
	newgamebtn->connect("pressed", &TMenuView::OnNewGameBtnClick, this);

	// join game
	tgui::Button::Ptr joingamebtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(joingamebtn);
	joingamebtn->setText("Join game");
	joingamebtn->setPosition(300, 300);
	joingamebtn->setSize(200, 60);
	joingamebtn->connect("pressed", &TMenuView::OnJoinGameBtnClick, this);

	// options
	tgui::Button::Ptr optionsbtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(optionsbtn);
	optionsbtn->setText("Options");
	optionsbtn->setPosition(25, 535);
	optionsbtn->setSize(100, 40);
	optionsbtn->connect("pressed", &TMenuView::OnOptionsBtnClick, this);

	// quit
	tgui::Button::Ptr quitbtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(quitbtn);
	quitbtn->setText("Quit");
	quitbtn->setPosition(675, 535);
	quitbtn->setSize(100, 40);
	quitbtn->connect("pressed", &TMenuView::OnQuitBtnClick, this);

	#ifdef _DEBUG
	// open other instance
	tgui::Button::Ptr newinstancebtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(newinstancebtn);
	newinstancebtn->setText("New instance");
	newinstancebtn->setPosition(675, 25);
	newinstancebtn->setSize(100, 40);
	newinstancebtn->connect("pressed", &TMenuView::OnNewInstanceBtnClick, this);
	#endif
}

void TMenuView::StartNewGame()
{
	Game->IsServer = true;
	Game->SwitchToState(GAMESTATE_LOBBY);
}

void TMenuView::OnNewGameBtnClick()
{
	StartNewGame();
}

void TMenuView::OnJoinGameBtnClick()
{
	/*
	tgui::MessageBox::Ptr msgbox = std::make_shared<tgui::MessageBox>();
	AddWidgetToGUI(msgbox);
	msgbox->setText("Not implemented yet");
	msgbox->setPosition(350, 275);
	msgbox->addButton("OK");
	msgbox->connect("ButtonPressed", [=](const std::string& button) {
		if (button == "OK")
			msgbox->hideWithEffect(tgui::ShowAnimationType::Fade, sf::Time::Zero);
		});
	msgbox->showWithEffect(tgui::ShowAnimationType::Fade, sf::Time::Zero);
	*/

	Game->SwitchToState(GAMESTATE_CONNECTTOSERVER);
}

void TMenuView::OnOptionsBtnClick()
{
	Game->SwitchToState(GAMESTATE_OPTIONS);
}

void TMenuView::OnQuitBtnClick()
{
	Game->RequestQuit();
}

#ifdef _DEBUG
void TMenuView::OnNewInstanceBtnClick()
{
	Game->StartNewInstance();
}
#endif

bool TMenuView::ProcessInput(TInputID InputID, float Value)
{
	// only handle key presses
	if (!IsInputPressed(Value))
		return false;

	bool handled = true;
	switch (InputID)
	{
		case actionToPreviousScreen:
			Game->RequestQuit();
			break;

		case actionDoDefaultAction:
			StartNewGame();
			break;

		case actionMenuJoinGame:
			Game->SwitchToState(GAMESTATE_CONNECTTOSERVER);
			break;

		case actionMenuOptions:
			Game->SwitchToState(GAMESTATE_OPTIONS);
			break;

		default:
			handled = false;
			break;
	};

	return handled;
}