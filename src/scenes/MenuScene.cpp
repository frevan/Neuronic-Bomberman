#include "menuscene.h"

#include <iomanip> 
#include "TGUI/TGUI.hpp"

#include "../base/nel_objecttypes.h"



TMenuScene::TMenuScene(std::shared_ptr<tgui::Gui> setGUI)
:	TTGUIScene(setGUI)
{
}

TMenuScene::~TMenuScene()
{
}

void TMenuScene::OnAttach(nel::IApplication* setApplication)
{
	TTGUIScene::OnAttach(setApplication);	

	createControls();
}

void TMenuScene::OnDetach()
{
	GUI->removeAllWidgets();

	TTGUIScene::OnDetach();
}

void TMenuScene::Draw(sf::RenderTarget* target)
{	
	tgui::Label::Ptr fpslbl = GUI->get<tgui::Label>("FPSLabel");
	if (fpslbl)
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << Application->GetCurrentFps();
		fpslbl->setText(ss.str() + " FPS");
		fpslbl->setPosition(800-fpslbl->getSize().x-25, 25);	
	}

	TTGUIScene::Draw(target);
}

void TMenuScene::createControls()
{
	GUI->removeAllWidgets();

	// game name label
	tgui::Label::Ptr namelbl = std::make_shared<tgui::Label>();
	GUI->add(namelbl);
	namelbl->setText("Neuronic Bomberman");
	namelbl->setPosition(25, 25);
	namelbl->setTextColor(sf::Color::Red);

	// FPS label
	tgui::Label::Ptr fpslbl = std::make_shared<tgui::Label>();
	GUI->add(fpslbl, "FPSLabel");
	fpslbl->setText("");
	fpslbl->setTextColor(sf::Color::Red);

	// new game
	tgui::Button::Ptr newgamebtn = std::make_shared<tgui::Button>();
	GUI->add(newgamebtn);
	newgamebtn->setText("New game");
	newgamebtn->setPosition(300, 200);
	newgamebtn->setSize(200, 60);
	newgamebtn->connect("pressed", std::bind(&TMenuScene::OnNewGameBtnClick, this));

	// join game
	tgui::Button::Ptr joingamebtn = std::make_shared<tgui::Button>();
	GUI->add(joingamebtn);
	joingamebtn->setText("Join game");
	joingamebtn->setPosition(300, 300);
	joingamebtn->setSize(200, 60);
	joingamebtn->connect("pressed", std::bind(&TMenuScene::OnJoinGameBtnClick, this));

	// options
	tgui::Button::Ptr optionsbtn = std::make_shared<tgui::Button>();
	GUI->add(optionsbtn);
	optionsbtn->setText("Options");
	optionsbtn->setPosition(25, 535);
	optionsbtn->setSize(100, 40);
	optionsbtn->connect("pressed", std::bind(&TMenuScene::OnOptionsBtnClick, this));

	// quit
	tgui::Button::Ptr quitbtn = std::make_shared<tgui::Button>();
	GUI->add(quitbtn);
	quitbtn->setText("Quit");
	quitbtn->setPosition(675, 535);
	quitbtn->setSize(100, 40);
	quitbtn->connect("pressed", std::bind(&TMenuScene::OnQuitBtnClick, this));
}

void TMenuScene::OnNewGameBtnClick()
{
}

void TMenuScene::OnJoinGameBtnClick()
{
}

void TMenuScene::OnOptionsBtnClick()
{
}

void TMenuScene::OnQuitBtnClick()
{
	Application->RequestQuit();
}
