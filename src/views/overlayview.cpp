#include "overlayview.h"

#include <iomanip> 
#include <TGUI/TGUI.hpp>

#include "../base/nel_objecttypes.h"
#include "../base/utility/nel_eventhandler.h"
#include "../base/utility/nel_macros.h"

#include "../states/states.h"



const std::string FPSLabelNameConst	= "FPSLabel";



DEFINE_GAMEID(actionToggleFPSLabel, "OverlayView::actionToggleFPSLabel")



TOverlayView::TOverlayView(std::shared_ptr<tgui::Gui> setGUI, nel::IStateMachine* setStateMachine)
:	TTGUIView(setGUI, setStateMachine, nel::IView::VT_SYSTEMVIEW)
{
	EventHandler = std::make_shared<nel::TEventHandler>(nel::IEventHandler::OVERLAY, nullptr, std::bind(&TOverlayView::ProcessInput, this, std::placeholders::_1, std::placeholders::_2));

	nel::TEventHandler* eh = (nel::TEventHandler*)EventHandler.get();
	eh->InputMap.DefineInput(actionToggleFPSLabel, nel::TInputControl::Pack(nel::TInputControl::TType::KEYBOARD, 0, sf::Keyboard::Key::Tab, 0));
}

TOverlayView::~TOverlayView()
{
	EventHandler.reset();
}

void TOverlayView::OnAttach(nel::IApplication* setApplication)
{
	TTGUIView::OnAttach(setApplication);
	Application->AddEventHandler(EventHandler);
}

void TOverlayView::OnDetach()
{
	Application->RemoveEventHandler(EventHandler);
}

void TOverlayView::Draw(sf::RenderTarget* target)
{	
	tgui::Label::Ptr fpslbl = GUI->get<tgui::Label>(FPSLabelNameConst);
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
	AddWidgetToGUI(fpslbl, FPSLabelNameConst);
	fpslbl->setText("");
	fpslbl->setTextColor(sf::Color::Red);
	fpslbl->setTextSize(12);
}

void TOverlayView::ProcessInput(nel::TGameID inputID, float value)
{
	if (inputID == actionToggleFPSLabel && value > 0)
	{
		tgui::Label::Ptr fpslbl = GUI->get<tgui::Label>(FPSLabelNameConst);
		if (fpslbl->isVisible())	fpslbl->hide();
		else						fpslbl->show();
	};
}