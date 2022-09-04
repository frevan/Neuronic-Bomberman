#pragma once

#include "../view.h"
#include "../gamedata.h"

class TLobbyView :	public TTGUIView
{
private:
	tgui::EditBox::Ptr GameNameEdit;
	tgui::ListView::Ptr PlayersListView;
	tgui::ComboBox::Ptr MapCombo;
	tgui::EditBox::Ptr NumRoundsEdit;
	std::vector<TArena> Maps;

	int SettingGameName;

	void OnBackBtnClick();
	void OnStartBtnClick();
	void OnMapComboItemSelected(int ItemIndex);
	void OnGameNameEditTextEntered(const std::string& Text);
	
	void FillMapCombo();	

	void StartMatchNow();

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