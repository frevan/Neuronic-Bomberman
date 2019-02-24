#include "menuview.h"

#include <iomanip> 
#include <TGUI/TGUI.hpp>

#include "../base/nel_objecttypes.h"

#include "../states/states.h"



TMenuView::TMenuView(std::shared_ptr<tgui::Gui> setGUI, nel::IStateMachine* setStateMachine)
:	TTGUIView(setGUI, setStateMachine)
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
}

void TMenuView::OnNewGameBtnClick()
{
	StateMachine->SetNextState(SID_ServerCreate, nullptr);
}

void TMenuView::OnJoinGameBtnClick()
{
	tgui::MessageBox::Ptr msgbox = std::make_shared<tgui::MessageBox>();
	AddWidgetToGUI(msgbox);
	msgbox->setText("Not implemented yet");
	msgbox->setPosition(350, 275);
	msgbox->addButton("OK");
	msgbox->showWithEffect(tgui::ShowAnimationType::Fade, sf::Time::Zero);

	//StateMachine->SetNextState(SID_ServerSelect);
}

void TMenuView::OnOptionsBtnClick()
{
	tgui::MessageBox::Ptr msgbox = std::make_shared<tgui::MessageBox>();
	AddWidgetToGUI(msgbox);
	msgbox->setText("Not implemented yet");
	msgbox->setPosition(350, 275);
	msgbox->showWithEffect(tgui::ShowAnimationType::Fade, sf::Time::Zero);

	//StateMachine->SetNextState(SID_Options);
}

void TMenuView::OnQuitBtnClick()
{
	Application->RequestQuit();
}
