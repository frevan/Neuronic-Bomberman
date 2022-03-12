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



TOverlayView::TOverlayView(nel::IStateMachine* setStateMachine)
:	TView(setStateMachine, nel::IView::TViewType::VT_OVERLAY),
	CommonFont(),
	EventHandler(),
	ShowOverlay(false),
	ShowFPS(true),
	ShowHelp(false),
	HelpLine1(),
	HelpLine2(),
	FPSText(),
	BackgroundShape()
{
	EventHandler = std::make_shared<nel::TEventHandler>(nel::IEventHandler::Type::OVERLAY, nullptr, std::bind(&TOverlayView::ProcessInput, this, std::placeholders::_1, std::placeholders::_2));

	nel::TEventHandler* eh = (nel::TEventHandler*)EventHandler.get();
	eh->InputMap.DefineInput(actionToggleFPSLabel, nel::TInputControl::Pack(nel::TInputControl::KEYBOARD, 0, sf::Keyboard::Key::Tab, 0));
	eh->InputMap.DefineInput(actionToggleHelpOverlay, nel::TInputControl::Pack(nel::TInputControl::KEYBOARD, 0, sf::Keyboard::Key::F1, 0));
}

TOverlayView::~TOverlayView()
{
	EventHandler.reset();
}

void TOverlayView::OnAttach(nel::IApplication* setApplication)
{
	TView::OnAttach(setApplication);
	Application->AddEventHandler(EventHandler);

	CommonFont.loadFromFile(Application->GetFontFileName());

	BackgroundShape.setFillColor(sf::Color(16, 16, 16, 200));
	BackgroundShape.setPosition(sf::Vector2f(0, 0));
	BackgroundShape.setSize(sf::Vector2f(800, 600));

	FPSText.setFont(CommonFont);
	FPSText.setFillColor(sf::Color::Red);
	FPSText.setOutlineColor(sf::Color::Red);
	FPSText.setCharacterSize(12);

	HelpLine1.setFont(CommonFont);
	HelpLine1.setString("F1 - toggle help display");
	HelpLine1.setCharacterSize(24);
	HelpLine1.setFillColor(sf::Color::White);
	HelpLine1.setOutlineColor(sf::Color::White);
	HelpLine1.setPosition(sf::Vector2f(25, 25));

	HelpLine2.setFont(CommonFont);
	HelpLine2.setString("TAB - toggle FPS counter");
	HelpLine2.setCharacterSize(24);
	HelpLine2.setFillColor(sf::Color::White);
	HelpLine2.setOutlineColor(sf::Color::White);
	HelpLine2.setPosition(sf::Vector2f(25, 60));
}

void TOverlayView::OnDetach()
{
	Application->RemoveEventHandler(EventHandler);

	TView::OnDetach();
}

void TOverlayView::Draw(sf::RenderTarget* target)
{
	TView::Draw(target);

	if (ShowOverlay)
		target->draw(BackgroundShape);

	if (ShowFPS || ShowOverlay)
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << Application->GetCurrentFps();
		FPSText.setString(ss.str() + " FPS");		
		sf::FloatRect bounds = FPSText.getLocalBounds();
		FPSText.setPosition(sf::Vector2f(800 - bounds.width - 25, 25));
		target->draw(FPSText);
	}

	if (ShowHelp && ShowOverlay)
	{
		target->draw(HelpLine1);
		target->draw(HelpLine2);
	}
}

void TOverlayView::ProcessInput(nel::TGameID inputID, float value)
{
	if (inputID == actionToggleFPSLabel && value > 0)
		ShowFPS = !ShowFPS;
	else if (inputID == actionToggleHelpOverlay && value > 0)
	{
		ShowHelp = !ShowHelp;
		ShowOverlay = ShowHelp;
	}
}