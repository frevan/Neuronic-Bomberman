#include "matchview.h"

#include "../actions.h"
#include "../resourcemgr.h"

TMatchView::TMatchView(TGame* SetGame)
:	TView(SetGame, TViewType::VT_HUMANVIEW)
{
}

TMatchView::~TMatchView()
{
}

void TMatchView::Draw(sf::RenderTarget* target)
{
	std::string s = "MATCH";
	sf::Text titleText(s, Game->Fonts.ByIndex(TFontManager::standard), 20);
	titleText.setPosition(5, 5);
	target->draw(titleText);
}

bool TMatchView::ProcessInput(TInputID InputID, float Value)
{
	bool handled = false;

	if (Value != 1.0f)
		return false; // only handle key presses

	switch (InputID)
	{
		case actionToPreviousScreen:
			Game->SwitchToState(STATE_MENU);
			handled = true;
			break;
	};

	return handled;
}
