#include "matchview.h"

#include "../game.h"
#include "../actions.h"
#include "../resourcemgr.h"

TMatchView::TMatchView(TGame* SetGame)
:	TView(SetGame, TViewType::VT_HUMANVIEW),
	BombAnimation()
{
}

TMatchView::~TMatchView()
{
}

void TMatchView::Draw(sf::RenderTarget* target)
{
	// ROUND x OF x
	std::string s = "ROUND " + std::to_string(Game->GameData.CurrentRound) + " OF " + std::to_string(Game->GameData.MaxRounds);
	sf::Text titleText(s, Game->Fonts.ByIndex(TFontManager::standard), 20);
	titleText.setPosition(5, 5);
	target->draw(titleText);

	s = Game->GameData.GameName;
	sf::Text statusText(s, Game->Fonts.ByIndex(TFontManager::standard), 15);
	statusText.setPosition(5, 30);
	target->draw(statusText);

	const int FIELD_SIZE = 45;
	float MapOffsetX = 62.f;
	float MapOffsetY = 65.f;

	// draw the field
	for (uint8_t y = 0; y < Game->GameData.Arena.Height; y++)
	{
		for (uint8_t x = 0; x < Game->GameData.Arena.Width; x++)
		{
			sf::Color fieldColor;

			TField field = Game->GameData.Arena.At(x, y);
			switch (field.Type)
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
			shape.setPosition(MapOffsetX + x * FIELD_SIZE, MapOffsetY + y * FIELD_SIZE);
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
		if (player->State == PLAYER_NOTPLAYING || player->State == PLAYER_DEAD)
			continue;

		float playerSize = FIELD_SIZE/2.f;

		sf::CircleShape shape(playerSize);
		shape.setPosition(MapOffsetX + player->Position.X * FIELD_SIZE - playerSize, MapOffsetY + player->Position.Y * FIELD_SIZE - playerSize);
		sf::Color playerColor;
		if (player->State == PLAYER_DYING)
			playerColor = sf::Color::White;
		else
			playerColor = Game->GameData.PlayerColors[idx];
		shape.setFillColor(playerColor);
		target->draw(shape);
	}

	// draw the bombs
	for (uint8_t x = 0; x < Game->GameData.Arena.Width; x++)
		for (uint8_t y = 0; y < Game->GameData.Arena.Height; y++)
		{
			TField field = Game->GameData.Arena.At(x, y);
			if (field.Bomb.State == BOMB_NONE)
				continue;

			TPlayerPosition pos;
			pos.X = x + 0.5f;
			pos.Y = y + 0.5f;

			float bombSize = FIELD_SIZE / 2.5f;
			sf::Color color = sf::Color::Black;	
			if (field.Bomb.State == BOMB_TICKING)
			{
				const float PulseInterval = 1250.f; // one complete pulse per 1.25 seconds
				float sine = sinf(fmodf(BombAnimation.Time * 1.f, PulseInterval) / PulseInterval * (float)M_PI * 2);
				bombSize += sine * (FIELD_SIZE / 25.f);
			}
			else if (field.Bomb.State == BOMB_EXPLODING)
				color = sf::Color::Red;

			sf::CircleShape shape(bombSize);
			shape.setPosition(MapOffsetX + pos.X * FIELD_SIZE - bombSize, MapOffsetY + pos.Y * FIELD_SIZE - bombSize);
			shape.setFillColor(color);
			target->draw(shape);
		}
}

void TMatchView::Process(TGameTime Delta)
{
	BombAnimation.Process(Delta);

	for (int slot = 0; slot < MAX_NUM_SLOTS; slot++)
	{
		TPlayer* p = &Game->GameData.Players[slot];
		if (p->State != PLAYER_ALIVE)
			continue;

		bool left = Game->InputMap.GetValueOfInput(actionMatchPlayer1Left + (slot * PlayerActionCount));
		bool right = Game->InputMap.GetValueOfInput(actionMatchPlayer1Right + (slot * PlayerActionCount));
		bool up = Game->InputMap.GetValueOfInput(actionMatchPlayer1Up + (slot * PlayerActionCount));
		bool down = Game->InputMap.GetValueOfInput(actionMatchPlayer1Down + (slot * PlayerActionCount));

		Game->Client->UpdatePlayerMovement(slot, left, right, up, down);
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
				Game->Client->CloseGame();
				Game->SwitchToState(GAMESTATE_MENU);
				handled = true;
			}
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

	/*
	if (InputID >= actionMatchPlayer1Left && InputID < actionMatchPlayer1Left + PlayerActionCount)
		PlayerIndex = 0;
	else if (InputID >= actionMatchPlayer2Left && InputID < actionMatchPlayer2Left + PlayerActionCount)
		PlayerIndex = 1;
	*/

	if (InputID >= actionMatchPlayer1Left && InputID < actionMatchPlayer1Left + PlayerActionCount * MAX_NUM_SLOTS)
		PlayerIndex = (InputID - actionMatchPlayer1Left) / PlayerActionCount;

	if (PlayerIndex >= 0)
		InputID = InputID - (PlayerActionCount * PlayerIndex);
}