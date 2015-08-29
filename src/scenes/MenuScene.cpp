#include "menuscene.h"

#include <iomanip> 
#include <TGUI/TGUI.hpp>

#include "../base/nel_objecttypes.h"

#include "../states/states.h"



TMenuScene::TMenuScene(std::shared_ptr<tgui::Gui> setGUI)
:	TTGUIScene(setGUI)
{
}

TMenuScene::~TMenuScene()
{
}

void TMenuScene::CreateWidgets()
{
	// game name label
	tgui::Label::Ptr namelbl = std::make_shared<tgui::Label>();
	AddWidgetToGUI(namelbl);
	namelbl->setText("Neuronic Bomberman");
	namelbl->setPosition(25, 25);
	namelbl->setTextColor(sf::Color::Red);
	namelbl->setTextSize(35);

	// new game
	tgui::Button::Ptr newgamebtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(newgamebtn);
	newgamebtn->setText("New game");
	newgamebtn->setPosition(300, 200);
	newgamebtn->setSize(200, 60);
	newgamebtn->connect("pressed", std::bind(&TMenuScene::OnNewGameBtnClick, this));

	// join game
	tgui::Button::Ptr joingamebtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(joingamebtn);
	joingamebtn->setText("Join game");
	joingamebtn->setPosition(300, 300);
	joingamebtn->setSize(200, 60);
	joingamebtn->connect("pressed", std::bind(&TMenuScene::OnJoinGameBtnClick, this));

	// options
	tgui::Button::Ptr optionsbtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(optionsbtn);
	optionsbtn->setText("Options");
	optionsbtn->setPosition(25, 535);
	optionsbtn->setSize(100, 40);
	optionsbtn->connect("pressed", std::bind(&TMenuScene::OnOptionsBtnClick, this));

	// quit
	tgui::Button::Ptr quitbtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(quitbtn);
	quitbtn->setText("Quit");
	quitbtn->setPosition(675, 535);
	quitbtn->setSize(100, 40);
	quitbtn->connect("pressed", std::bind(&TMenuScene::OnQuitBtnClick, this));
}

void TMenuScene::OnNewGameBtnClick()
{
	Application->SetNextState(SID_Lobby);
}

void TMenuScene::OnJoinGameBtnClick()
{
	//Application->SetNextState(SID_ServerSelect);
}

void TMenuScene::OnOptionsBtnClick()
{
	//Application->SetNextState(SID_Options);
}

void TMenuScene::OnQuitBtnClick()
{
	Application->RequestQuit();
}
