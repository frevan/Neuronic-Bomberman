#pragma once

#include "../view.h"



class TMenuView : public TTGUIView
{
private:	
	void CreateWidgets() override;

	void OnNewGameBtnClick();
	void OnJoinGameBtnClick();
	void OnOptionsBtnClick();
	void OnQuitBtnClick();

	void StartNewGame();

public:
	TMenuView(TGame* SetGame, tgui::Gui* SetGUI);
	~TMenuView();

	// from TView
	bool TMenuView::ProcessInput(TInputID InputID, float Value) override;
};