#pragma once

#include "../view.h"
#include "../gamedata.h"

class TLobbyView :	public TTGUIView
{
private:
	tgui::ListView::Ptr PlayersListView;
	tgui::ComboBox::Ptr MapCombo;
	std::vector<TArena> Maps;

	void OnBackBtnClick();
	void OnStartBtnClick();
	
	void FillMapCombo();
	void OnMapComboItemSelected(int ItemIndex);

protected:
	void CreateWidgets() override;
	void LeaveLobby();

public:
	TLobbyView(TGame* SetGame, tgui::Gui* SetGUI);
	~TLobbyView();

	bool ProcessInput(TInputID InputID, float Value) override;
	void StateChanged() override;
	void Draw(sf::RenderTarget* target) override;
};