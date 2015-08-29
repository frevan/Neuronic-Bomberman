#include "overlayscene.h"

#include <iomanip> 
#include <TGUI/TGUI.hpp>

#include "../base/nel_objecttypes.h"

#include "../states/states.h"



TOverlayScene::TOverlayScene(std::shared_ptr<tgui::Gui> setGUI)
:	TTGUIScene(setGUI, nel::IScene::VT_SYSTEMVIEW)
{
}

TOverlayScene::~TOverlayScene()
{
}

void TOverlayScene::Draw(sf::RenderTarget* target)
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

void TOverlayScene::CreateWidgets()
{
	// FPS label
	tgui::Label::Ptr fpslbl = std::make_shared<tgui::Label>();
	AddWidgetToGUI(fpslbl, "FPSLabel");
	fpslbl->setText("");
	fpslbl->setTextColor(sf::Color::Red);
	fpslbl->setTextSize(12);
}