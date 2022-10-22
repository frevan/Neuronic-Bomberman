#include "connectview.h"

#include <TGUI/TGUI.hpp>

#include "../actions.h"
#include "../comms.h"

TConnectToServerView::TConnectToServerView(TGame* SetGame, tgui::Gui* SetGUI)
	: TTGUIView(SetGame, TViewType::VT_HUMANVIEW, SetGUI)
{
}

TConnectToServerView::~TConnectToServerView()
{
}

void TConnectToServerView::CreateWidgets()
{
	// game name label
	tgui::Label::Ptr addresslbl = std::make_shared<tgui::Label>();
	AddWidgetToGUI(addresslbl);
	addresslbl->setText("127.0.0.1");
	addresslbl->setPosition(310, 200);
	addresslbl->getRenderer()->setTextColor(sf::Color::Red);
	addresslbl->setTextSize(35);

	// new game
	tgui::Button::Ptr connectbtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(connectbtn);
	connectbtn->setText("Connect");
	connectbtn->setPosition(300, 260);
	connectbtn->setSize(200, 60);
	connectbtn->connect("pressed", &TConnectToServerView::OnConnectBtnClick, this);
}

void TConnectToServerView::OnConnectBtnClick()
{
	Connect();
}
bool TConnectToServerView::ProcessInput(TInputID InputID, float Value)
{
	bool handled = false;

	if (Value != 1.0f)
		return false; // only handle key presses

	switch (InputID)
	{
		case actionToPreviousScreen:
			Game->SwitchToState(GAMESTATE_MENU);
			handled = true;
			break;

		case actionDoDefaultAction:
			Connect();
			break;
	};

	return handled;
}

void TConnectToServerView::StateChanged()
{
}

void TConnectToServerView::Connect()
{
	Game->IsServer = false;
	Game->Client->Connect("127.0.0.1", SERVER_PORT);
}