#pragma once

#include "../view.h"
#include "../gamedata.h"

class TLobbyView :	public TTGUIView
{
private:
	tgui::EditBox::Ptr GameNameEdit;
	tgui::ComboBox::Ptr MapCombo;
	tgui::EditBox::Ptr NumRoundsEdit;
	std::vector<TArena> Maps;
	int SelectedSlot;

	int SettingGameName;

	void OnBackBtnClick();
	void OnStartBtnClick();
	void OnMapComboItemSelected(int ItemIndex);
	void OnGameNameEditTextEntered(const std::string& Text);
	void OnAddPlayerBtnClick();
	void OnRemovePlayerBtnClick();
	void OnRemapPlayerControlsBtnClick();
	
	void FillMapCombo();
	void DoAddPlayer();
	void DoRemovePlayer();
	void DoRemapPlayerControls();

	void StartMatchNow();

	const std::string CreatePlayerShortcutString(int Slot);
	const std::string ControlToString(TInputControl::TPacked Control);

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