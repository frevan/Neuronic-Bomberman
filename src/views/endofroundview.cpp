#include "endofroundview.h"

#include "../actions.h"
#include "../resourcemgr.h"

TEndOfRoundView::TEndOfRoundView(TGame* SetGame)
:	TView(SetGame, TViewType::VT_HUMANVIEW)
{
}

TEndOfRoundView::~TEndOfRoundView()
{
}

void TEndOfRoundView::Draw(sf::RenderTarget* target)
{
	std::string s = "ROUND ENDED";
	sf::Text titleText(s, Game->Fonts.ByIndex(TFontManager::standard), 20);
	titleText.setPosition(5, 5);
	target->draw(titleText);
}

bool TEndOfRoundView::ProcessInput(TInputID InputID, float Value)
{
	bool handled = false;

	int playerIndex = -1;

	switch (InputID)
	{
		case actionToPreviousScreen:
			if (Value == 1.0f)
			{
				Game->SwitchToState(STATE_MENU);
				handled = true;
			}
			break;
	};

	return handled;
}