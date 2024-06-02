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

	unsigned int highestScore = 0;
	for (int idx = 0; idx < MAX_NUM_SLOTS; idx++)
		if (Game->GameData.Players[idx].RoundsWon > highestScore)
			highestScore = Game->GameData.Players[idx].RoundsWon;

	// show players with score
	float playerListY = 100.f;
	for (int idx = 0; idx < MAX_NUM_SLOTS; idx++)
	{
		TPlayer* p = &Game->GameData.Players[idx];
		if (p->State == PLAYER_NOTPLAYING)
			continue;

		int textHeight = 30;
		if (p->RoundsWon == highestScore)
			textHeight = 50;

		s = std::to_string(idx + 1) + ". " + p->Name + " ... " + std::to_string(p->RoundsWon);
		sf::Text playerText(s, Game->Fonts.ByIndex(TFontManager::standard), textHeight);
		playerText.setPosition(75, playerListY);
		target->draw(playerText);

		playerListY += textHeight + 10.f;
	}
}

bool TEndOfRoundView::ProcessInput(TInputID InputID, float Value)
{
	bool handled = true;
	switch (InputID)
	{
		case actionToPreviousScreen:
			if (IsInputPressed(Value))
				Game->SwitchToState(GAMESTATE_MENU);
			break;

		case actionDoDefaultAction:
			if (IsInputPressed(Value))
			{
				if (Game->GameData.CurrentRound >= Game->GameData.MaxRounds)
					Game->SwitchToState(GAMESTATE_MENU);
				else
					Game->Client->StartNextRound();
			}
			break;

		default:
			handled = false;
			break;
	};

	return handled;
}