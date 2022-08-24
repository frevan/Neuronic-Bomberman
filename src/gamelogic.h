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
public:
	TGameLogic(TGame* SetGame);
	~TGameLogic();

	void Process(TGameTime Delta);
};