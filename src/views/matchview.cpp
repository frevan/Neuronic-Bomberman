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

	s = "STARTED";
	switch (Game->GameData.Status)
	{
		case GAME_STARTING: s = "starting"; break;
		case GAME_RUNNING: s = "started"; break;
		default: s = "error"; break;
	}
	sf::Text statusText(s, Game->Fonts.ByIndex(TFontManager::standard), 15);
	statusText.setPosition(5, 30);
	target->draw(statusText);

	const int FIELD_SIZE = 45;
	float MapOffsetX = 62.f;
	float MapOffsetY = 65.f;

	// draw the field
	for (size_t row = 0; row < Game->GameData.Arena.Height; row++)
	{
		for (size_t col = 0; col < Game->GameData.Arena.Width; col++)
		{
			sf::Color fieldColor;

			TField* field = Game->GameData.Arena.At((uint8_t)col, (uint8_t)row);
			switch (field->Type)
			{
				case FIELD_EMPTY: fieldColor = sf::Color::Green; break;
				case FIELD_BRICK: fieldColor = sf::Color::Red; break;
				case FIELD_SOLID: 
					fieldColor.r = 128;
					fieldColor.g = 128;
					fieldColor.b = 128;
					break;
			};

			sf::RectangleShape shape(sf::Vector2f(FIELD_SIZE, FIELD_SIZE));
			shape.setPosition(MapOffsetX + col * FIELD_SIZE, MapOffsetY + row * FIELD_SIZE);
			shape.setFillColor(fieldColor);
			shape.setOutlineColor(sf::Color::White);
			shape.setOutlineThickness(1.f);
			target->draw(shape);
		}
	}

	// draw the players
	for (int idx = 0; idx < MAX_NUM_SLOTS; idx++)
	{
		TPlayer* player = &Game->GameData.Players[idx];
		if (player->State == PLAYER_NOTPLAYING)
			continue;

		float playerSize = FIELD_SIZE/2.f;

		sf::CircleShape shape(playerSize);
		shape.setPosition(MapOffsetX + player->Position.X * FIELD_SIZE - playerSize, MapOffsetY + player->Position.Y * FIELD_SIZE - playerSize);
		switch (idx)
		{
			case 0: shape.setFillColor(sf::Color::Blue); break;
			case 1: shape.setFillColor(sf::Color::Magenta); break;
		}
		target->draw(shape);
	}

	// draw the bombs
	for (auto it = Game->GameData.Bombs.begin(); it != Game->GameData.Bombs.end(); it++)
	{
		TPlayerPosition pos;
		pos.X = (*it).Position.X + 0.5f;
		pos.Y = (*it).Position.Y + 0.5f;

		float bombSize = FIELD_SIZE / 3.f;

		sf::CircleShape shape(bombSize);
		shape.setPosition(MapOffsetX + pos.X * FIELD_SIZE - bombSize, MapOffsetY + pos.Y * FIELD_SIZE - bombSize);
		shape.setFillColor(sf::Color::Black);
		target->draw(shape);
	}
}

bool TMatchView::ProcessInput(TInputID InputID, float Value)
{
	bool handled = false;

	int playerIndex = -1;
	DeterminePlayerActionFromInput(InputID, Value, playerIndex);

	switch (InputID)
	{
		case actionToPreviousScreen:
			if (Value == 1.0f)
			{
				Game->SwitchToState(STATE_MENU);
				handled = true;
			}
			break;

		case actionMatchPlayer1Left:
			Game->Client->UpdatePlayerMovement(playerIndex, DIRECTION_LEFT, Value > 0.f);
			handled = true;
			break;
		case actionMatchPlayer1Right:
			Game->Client->UpdatePlayerMovement(playerIndex, DIRECTION_RIGHT, Value > 0.f);
			handled = true;
			break;
		case actionMatchPlayer1Up:
			Game->Client->UpdatePlayerMovement(playerIndex, DIRECTION_UP, Value > 0.f);
			handled = true;
			break;
		case actionMatchPlayer1Down:
			Game->Client->UpdatePlayerMovement(playerIndex, DIRECTION_DOWN, Value > 0.f);
			handled = true;
			break;
		case actionMatchPlayer1DropBomb:
			Game->Client->DropBomb(playerIndex);
			break;
	};

	return handled;
}

void TMatchView::DeterminePlayerActionFromInput(TInputID& InputID, float& Value, int& PlayerIndex)
{
	PlayerIndex = -1;

	if (InputID >= actionMatchPlayer1Left && InputID < actionMatchPlayer1Left + PlayerActionCount)
		PlayerIndex = 0;
	else if (InputID >= actionMatchPlayer2Left && InputID < actionMatchPlayer2Left + PlayerActionCount)
		PlayerIndex = 1;

	if (PlayerIndex >= 0)
		InputID = InputID - (PlayerActionCount * PlayerIndex);
}