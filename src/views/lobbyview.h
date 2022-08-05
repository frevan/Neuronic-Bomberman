#pragma once

#include "../view.h"

class TLobbyView :	public TTGUIView
{
private:
	tgui::ListView::Ptr PlayersListView;

	void OnBackBtnClick();
	void OnStartBtnClick();

protected:
	void CreateWidgets() override;
	void LeaveLobby();

public:
	TLobbyView(TGame* SetGame, tgui::Gui* SetGUI);
	~TLobbyView();

	bool ProcessInput(TInputID InputID, float Value) override;
	void StateChanged() override;
};