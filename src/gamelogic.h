#pragma once

#include "gamedata.h"

class TLogicListener
{
public:
	virtual void LogicBombExploding(const TFieldPosition& FieldPosition) = 0;
	virtual void LogicBombExploded(const TFieldPosition& FieldPosition) = 0;
	virtual void LogicPlayerDying(uint8_t Slot) = 0;
	virtual void LogicPlayerDied(uint8_t Slot) = 0;
	virtual void LogicRoundEnded() = 0;
};

class TGameLogic
{
private:
	TGameData* Data;
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
	TGameLogic(TGameData* SetData, TLogicListener* SetListener);
	void Process(TGameTime Delta);
};