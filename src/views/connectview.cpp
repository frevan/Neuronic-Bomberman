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
	// address label
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
	AddressEdit->setPosition(80, 230);
	AddressEdit->setSize(420, 70);
	AddressEdit->setTextSize(50);
	AddressEdit->setAlignment(tgui::EditBox::Alignment::Center);
	// previous adress labels
	for (int i = 0; i < 5; i++)
	{
		PreviousAddressLabels[i] = std::make_shared<tgui::Label>();
		AddWidgetToGUI(PreviousAddressLabels[i]);
		PreviousAddressLabels[i]->setPosition(80, 320 + i * 38);
		PreviousAddressLabels[i]->setSize(420, 35);
		PreviousAddressLabels[i]->setTextSize(30);
		PreviousAddressLabels[i]->getRenderer()->setTextColor(sf::Color::White);
	}
	UpdatePreviousAddressList();

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
	std::string txt = AddressEdit->getText();
	trim(txt);
	if (txt.empty())
		return;

	Connect(txt);
}
bool TConnectToServerView::ProcessInput(TInputID InputID, float Value)
{
	if (!IsInputPressed(Value))  // only handle key presses
		return false;

	int idx;
	std::string address;

	bool handled = true;
	switch (InputID)
	{
		case actionToPreviousScreen:
			Game->SwitchToState(GAMESTATE_MENU);
			break;

		case actionDoDefaultAction:
			address = AddressEdit->getText();
			trim(address);
			if (!address.empty())
				Connect(address);
			break;

		case actionConnectPrevious1:
		case actionConnectPrevious2:
		case actionConnectPrevious3:
		case actionConnectPrevious4:
		case actionConnectPrevious5:
			idx = InputID - actionConnectPrevious1 + 1;
			address = Game->GetPreviousServerAddress(idx);
			if (!address.empty())
				Connect(address);
			break;

		default:
			handled = false;
			break;
	};

	return handled;
}

void TConnectToServerView::StateChanged()
{
}

void TConnectToServerView::Connect(const std::string Address)
{
	sf::IpAddress ip(Address);
	if (ip == sf::IpAddress::None)
		return;

	Game->SetChosenServerAddress(ip.toString());

	Game->IsServer = false;
	Game->Client->Connect(Game->GetChosenServerAddress(), SERVER_PORT);

	UpdatePreviousAddressList();
}

void TConnectToServerView::UpdatePreviousAddressList()
{
	std::string address;

	address = Game->GetChosenServerAddress();
	AddressEdit->setText(address);

	for (int i = 0; i < 5; i++)
	{
		address = Game->GetPreviousServerAddress(i + 1);
		if (address.empty())
			PreviousAddressLabels[i]-> setText("");
		else
			PreviousAddressLabels[i]->setText("[F" + std::to_string(i + 1) + "] " + address);
	}
}