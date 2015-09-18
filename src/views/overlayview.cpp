#include "overlayview.h"

#include <iomanip> 
#include <TGUI/TGUI.hpp>

#include "../base/nel_objecttypes.h"

#include "../states/states.h"



TOverlayView::TOverlayView(std::shared_ptr<tgui::Gui> setGUI, nel::IStateMachine* setStateMachine)
:	TTGUIView(setGUI, setStateMachine, nel::IView::VT_SYSTEMVIEW)
{
}

TOverlayView::~TOverlayView()
{
}

void TOverlayView::Draw(sf::RenderTarget* target)
{	
	tgui::Label::Ptr fpslbl = GUI->get<tgui::Label>("FPSLabel");
	if (fpslbl)
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << Application->GetCurrentFps();
		fpslbl->setText(ss.str() + " FPS");
		fpslbl->setPosition(800-fpslbl->getSize().x-25, 25);	
	}

	TTGUIView::Draw(target);
}

void TOverlayView::CreateWidgets()
{
	// FPS label
	tgui::Label::Ptr fpslbl = std::make_shared<tgui::Label>();
	AddWidgetToGUI(fpslbl, "FPSLabel");
	fpslbl->setText("");
	fpslbl->setTextColor(sf::Color::Red);
	fpslbl->setTextSize(12);
}