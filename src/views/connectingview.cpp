#include "connectingview.h"

#include <TGUI/TGUI.hpp>

#include "../base/nel_objecttypes.h"

#include "../states/states.h"



TConnectingView::TConnectingView(std::shared_ptr<tgui::Gui> setGUI, nel::IStateMachine* setStateMachine)
:	TTGUIView(setGUI, setStateMachine)
{
}

TConnectingView::~TConnectingView()
{
}

void TConnectingView::CreateWidgets()
{
	// server name
	tgui::Label::Ptr servernamelbl = std::make_shared<tgui::Label>();
	AddWidgetToGUI(servernamelbl);
	servernamelbl->setText("Connecting...");
	servernamelbl->setPosition(25, 25);
	servernamelbl->getRenderer()->setTextColor(sf::Color::White);
	servernamelbl->setTextSize(14);
}
