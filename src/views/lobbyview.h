#pragma once

#include "../view.h"
#include "../gamedata.h"

class TLobbyView :	public TTGUIView
{
private:
	tgui::EditBox::Ptr GameNameEdit;
	tgui::Label::Ptr GameNameLabel;
	tgui::ComboBox::Ptr MapCombo;
	tgui::Label::Ptr MapNameLabel;
	tgui::SpinButton::Ptr NumRoundsSpin;
	tgui::Label::Ptr NumRoundsLabel;
	std::vector<TArena> Maps;
	int SelectedSlot;

	int SettingGameData;

	void OnBackBtnClick();
	void OnStartBtnClick();
	void OnMapComboItemSelected(int ItemIndex);
	void OnGameNameEditTextEntered(const std::string& Text);
	void OnAddPlayerBtnClick();
	void OnRemovePlayerBtnClick();
	void OnRemapPlayerControlsBtnClick();
	void OnNumRoundsSpinChanged(float NewValue);
	
	void FillMapCombo(bool KeepSelectedItem);
	void SetMapLabelText();
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