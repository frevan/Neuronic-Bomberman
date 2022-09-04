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
	std::string s;
	if (Game->GameData.CurrentRound == Game->GameData.MaxRounds)
		s = "END OF MATCH";
	else
		s = "END OF ROUND " + std::to_string(Game->GameData.CurrentRound);
	sf::Text titleText(s, Game->Fonts.ByIndex(TFontManager::standard), 20);
	titleText.setPosition(5, 5);
	target->draw(titleText);

	// show players with score
	int playerListTop = 100;
	for (int idx = 0; idx < MAX_NUM_SLOTS; idx++)
	{
		TPlayer* p = &Game->GameData.Players[idx];
		if (p->State == PLAYER_NOTPLAYING)
			continue;

		s = std::to_string(idx + 1) + ". " + p->Name + " " + std::to_string(p->RoundsWon);
		sf::Text playerText(s, Game->Fonts.ByIndex(TFontManager::standard), 30);
		playerText.setPosition(75, playerListTop + idx * 40.f);
		target->draw(playerText);
	}
}

bool TEndOfRoundView::ProcessInput(TInputID InputID, float Value)
{
	bool handled = false;

	switch (InputID)
	{
		case actionToPreviousScreen:
			if (Value == 1.0f)
			{
				Game->SwitchToState(STATE_MENU);
				handled = true;
			}
			break;

		case actionDoDefaultAction:
			if (Value == 1.0f)
			{
				if (Game->GameData.CurrentRound >= Game->GameData.MaxRounds)
					Game->SwitchToState(STATE_MENU);
				else
					Game->SwitchToState(STATE_MATCH);
				handled = true;
			}
			break;
	};

	return handled;
}