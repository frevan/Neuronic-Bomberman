#pragma once

class TGameLogic;

class TLogicListener
{
public:
	virtual void LogicRoundEnded() = 0;
};

#include "game.h"
#include "gamedata.h"

class TGameLogic
{
private:
	TGame* Game;
	TLogicListener* Listener;

	float MovePlayer(TPlayer* Player, TPlayerDirection Direction, float Distance, bool Recurse = true);
	float CanMove(TPlayer* Player, TPlayerDirection Direction, float Distance, TPlayerDirection& OtherDirection);
	void AdjustPlayerPos(TPlayer* Player, TPlayerDirection Direction, float Distance);
	bool CheckMoveAgainstCell(TPlayer* Player, int CellX, int CellY, TPlayerDirection& Direction, TPlayerDirection& OtherDirection, bool Recurse);
	void UpdatePlayerPositions(TGameTime Delta);
	void UpdateBombs(TGameTime Delta);
	void ExplodeBomb(uint8_t X, uint8_t Y);
	void ExplodeField(uint8_t X, uint8_t Y, const TBomb& OriginalBomb, bool& Stop);
	void CheckForExplodedPlayers();
	void UpdateDyingPlayers(TGameTime Delta);
	void EndRound();
	TFieldPosition CalculatePlayerField(TPlayer* Player);
public:
	TGameLogic(TGame* SetGame, TLogicListener* SetListener);
	~TGameLogic();

	void Process(TGameTime Delta);
};