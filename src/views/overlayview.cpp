#include "overlayview.h"

#include <iomanip> 
#include <TGUI/TGUI.hpp>

#include "../base/nel_objecttypes.h"
#include "../base/utility/nel_eventhandler.h"
#include "../base/utility/nel_macros.h"

#include "../states/states.h"



//const std::string FPSLabelNameConst	= "FPSLabel";

const std::string HelpHelpLabelNameConst	= "HelpHelpLabel";
const std::string HelpFPSLabelNameConst		= "HelpFPSLabel";
const std::string HelpExitLabelNameConst	= "HelpExitLabel";



DEFINE_GAMEID(actionToggleFPSLabel, "OverlayView::actionToggleFPSLabel")
DEFINE_GAMEID(actionToggleHelpOverlay, "OverlayView::actionToggleHelpOverlay")
DEFINE_GAMEID(actionToggleOverlay, "OverlayView::actionToggleOverlay")



TOverlayView::TOverlayView(std::shared_ptr<tgui::Gui> setGUI, nel::IStateMachine* setStateMachine)
:	TTGUIView(setGUI, setStateMachine, nel::IView::VT_OVERLAY),
	CommonFont(),
	EventHandler(),
	ShowOverlay(false),
	ShowFPS(true)
{
	EventHandler = std::make_shared<nel::TEventHandler>(nel::IEventHandler::OVERLAY, nullptr, std::bind(&TOverlayView::ProcessInput, this, std::placeholders::_1, std::placeholders::_2));

	nel::TEventHandler* eh = (nel::TEventHandler*)EventHandler.get();
	eh->InputMap.DefineInput(actionToggleFPSLabel, nel::TInputControl::Pack(nel::TInputControl::KEYBOARD, 0, sf::Keyboard::Key::Tab, nel::TInputControl::SHIFT));
	eh->InputMap.DefineInput(actionToggleHelpOverlay, nel::TInputControl::Pack(nel::TInputControl::KEYBOARD, 0, sf::Keyboard::Key::F1, 0));
	eh->InputMap.DefineInput(actionToggleOverlay, nel::TInputControl::Pack(nel::TInputControl::KEYBOARD, 0, sf::Keyboard::Key::Tab, 0));
}

TOverlayView::~TOverlayView()
{
	EventHandler.reset();
}

void TOverlayView::OnAttach(nel::IApplication* setApplication)
{
	TTGUIView::OnAttach(setApplication);
	Application->AddEventHandler(EventHandler);

	CommonFont.loadFromFile(Application->GetFontFileName());
}

void TOverlayView::OnDetach()
{
	Application->RemoveEventHandler(EventHandler);

	TTGUIView::OnDetach();
}

void TOverlayView::Draw(sf::RenderTarget* target)
{
	TTGUIView::Draw(target);

	if (ShowOverlay)
	{
		sf::RectangleShape r;
		r.setFillColor(sf::Color(0, 0, 0, 150));
		r.setPosition(sf::Vector2f(0, 0));
		r.setSize(sf::Vector2f(800, 600));
		target->draw(r);
	}

	if (ShowFPS | ShowOverlay)
	{
		sf::Text fpstext;
		fpstext.setFont(CommonFont);
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << Application->GetCurrentFps();
		fpstext.setString(ss.str() + " FPS");
		fpstext.setCharacterSize(12);
		fpstext.setColor(sf::Color::Red);
		sf::FloatRect bounds = fpstext.getLocalBounds();
		fpstext.setPosition(sf::Vector2f(800 - bounds.width - 25, 25));
		target->draw(fpstext);
	}
}

void TOverlayView::CreateWidgets()
{
	// help labels
	CreateHelpLabels();
}

void TOverlayView::CreateHelpLabels()
{
	tgui::Label::Ptr lbl = std::make_shared<tgui::Label>();
	AddWidgetToGUI(lbl, HelpHelpLabelNameConst);
	lbl->setText("F1 - show this help");
	lbl->setTextColor(sf::Color::Blue);
	lbl->setTextSize(12);
	lbl->setPosition(25, 25);
	lbl->hide();

	lbl = std::make_shared<tgui::Label>();
	AddWidgetToGUI(lbl, HelpFPSLabelNameConst);
	lbl->setText("Tab - show FPS label");
	lbl->setTextColor(sf::Color::Blue);
	lbl->setTextSize(12);
	lbl->setPosition(25, 50);
	lbl->hide();

	lbl = std::make_shared<tgui::Label>();
	AddWidgetToGUI(lbl, HelpExitLabelNameConst);
	lbl->setText("Esc - exit this screen");
	lbl->setTextColor(sf::Color::Blue);
	lbl->setTextSize(12);
	lbl->setPosition(25, 75);
	lbl->hide();
}

void TOverlayView::ProcessInput(nel::TGameID inputID, float value)
{
	if (inputID == actionToggleFPSLabel && value > 0)
		ShowFPS = !ShowFPS;
	else if (inputID == actionToggleHelpOverlay && value > 0)
	{
		tgui::Label::Ptr helplbl = GUI->get<tgui::Label>(HelpHelpLabelNameConst);
		tgui::Label::Ptr fpslbl = GUI->get<tgui::Label>(HelpFPSLabelNameConst);
		tgui::Label::Ptr exitlbl = GUI->get<tgui::Label>(HelpExitLabelNameConst);
		bool shouldhide = helplbl->isVisible();		
		if (shouldhide)
		{
			helplbl->hide();
			fpslbl->hide();
			exitlbl->hide();
		}
		else
		{
			helplbl->show();
			fpslbl->show();
			exitlbl->show();
		}
	}
	else if (inputID == actionToggleOverlay && value > 0)
		ShowOverlay = !ShowOverlay;
}