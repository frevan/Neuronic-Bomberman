#include "menuscene.h"

#include "TGUI/TGUI.hpp"



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
	TTGUIScene::Draw(target);

	sf::CircleShape shape(75);
	shape.setPosition(sf::Vector2f(300, 100));
	shape.setFillColor(sf::Color::Red);
	target->draw(shape);
}

void TMenuScene::createControls()
{
	GUI->removeAllWidgets();

	tgui::Button::Ptr quitbtn = std::make_shared<tgui::Button>();
	GUI->add(quitbtn);
	quitbtn->setText("Quit");
	quitbtn->setPosition(tgui::Layout2d(25, 25));
	quitbtn->setSize(tgui::Layout2d(100, 50));
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
