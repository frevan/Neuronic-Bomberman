#include "menuscene.h"



TMenuScene::TMenuScene()
:	TSFGUIScene()
{
}

TMenuScene::~TMenuScene()
{
}

void TMenuScene::onAttach(nel::IApplication* setApplication)
{
	TSFGUIScene::onAttach(setApplication);
	Application->addEventHandler(this);

	createControls();	
}

void TMenuScene::onDetach()
{
	Application->removeEventHandler(this);
	TSFGUIScene::onDetach();
}

void TMenuScene::createControls()
{
	// create the label.
	auto label = sfg::Label::Create("Hello world!");

	// create a simple button and connect the click signal.
	auto button = sfg::Button::Create("Quit");
	button->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&TMenuScene::onButtonClick, this));

	// create a vertical box layouter with 5 pixels spacing and add the label and button to it.
	auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
	box->Pack(label);
	box->Pack(button, false);

	// add the box to the desktop
	Desktop->Add(box);	
}

void TMenuScene::onButtonClick()
{
	Application->requestQuit();
}
