#pragma once

class TGameLogic;

#include "game.h"
#include "gamedata.h"

class TGameLogic
{
private:
	TGame* Game;
	float MovePlayer(TPlayer* Player, TPlayerDirection Direction, float Distance, bool Recurse = true);
	float CanMove(TPlayer* Player, TPlayerDirection Direction, float Distance, TPlayerDirection& OtherDirection);
	void AdjustPlayerPos(TPlayer* Player, TPlayerDirection Direction, float Distance);
	bool CheckMoveAgainstCell(TPlayer* Player, int CellX, int CellY, TPlayerDirection& Direction, TPlayerDirection& OtherDirection, bool Recurse);
	void UpdatePlayerPositions(TGameTime Delta);
	void UpdateBombs(TGameTime Delta);
	void ExplodeBomb(uint8_t X, uint8_t Y);
	void CheckForExplodedPlayers();
	void UpdateDyingPlayers(TGameTime Delta);
public:
	TGameLogic(TGame* SetGame);
	~TGameLogic();

	void Process(TGameTime Delta);
};