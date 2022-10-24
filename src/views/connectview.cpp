#include "connectview.h"

#include <TGUI/TGUI.hpp>

#include "../actions.h"
#include "../comms.h"
#include "../utility/stringutil.h"

TConnectToServerView::TConnectToServerView(TGame* SetGame, tgui::Gui* SetGUI)
	: TTGUIView(SetGame, TViewType::VT_HUMANVIEW, SetGUI),
	AddressEdit(nullptr),
	ConnectBtn(nullptr)
{
}

TConnectToServerView::~TConnectToServerView()
{
}

void TConnectToServerView::CreateWidgets()
{
	// addess label
	tgui::Label::Ptr addressLabel = std::make_shared<tgui::Label>();
	AddWidgetToGUI(addressLabel);
	addressLabel->setText("server address");
	addressLabel->setPosition(80, 210);
	addressLabel->setSize(100, 15);
	addressLabel->setTextSize(12);
	addressLabel->getRenderer()->setTextColor(sf::Color::White);

	// address input
	AddressEdit = std::make_shared<tgui::EditBox>();
	AddWidgetToGUI(AddressEdit);
	AddressEdit->setText(Game->ChosenServerAddress);
	AddressEdit->setPosition(80, 230);
	AddressEdit->setSize(420, 70);
	AddressEdit->setTextSize(50);
	AddressEdit->setAlignment(tgui::EditBox::Alignment::Center);

	// connect btn
	ConnectBtn = std::make_shared<tgui::Button>();
	AddWidgetToGUI(ConnectBtn);
	ConnectBtn->setText("Connect");
	ConnectBtn->setPosition(520, 230);
	ConnectBtn->setSize(190, 70);
	ConnectBtn->setTextSize(30);
	ConnectBtn->connect("pressed", &TConnectToServerView::OnConnectBtnClick, this);

	AddressEdit->setFocused(true);
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
	std::string txt = AddressEdit->getText();
	trim(txt);
	if (txt.empty())
		return;

	sf::IpAddress address(txt);
	if (address == sf::IpAddress::None)
		return;

	Game->ChosenServerAddress = address.toString();

	Game->IsServer = false;
	Game->Client->Connect(Game->ChosenServerAddress, SERVER_PORT);
}