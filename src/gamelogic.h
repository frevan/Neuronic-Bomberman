#pragma once

#include "gamedata.h"

#include <mutex>

class TLogicListener
{
public:
	virtual void LogicBombExploding(const TFieldPosition& FieldPosition) = 0;
	virtual void LogicBombExploded(const TFieldPosition& FieldPosition) = 0;
	virtual void LogicPlayerDying(uint8_t Slot) = 0;
	virtual void LogicPlayerDied(uint8_t Slot) = 0;
	virtual void LogicRoundEnded() = 0;
};

typedef enum { actionChangeDirection, actionDropBomb, actionKick } TPlayerActionType;

typedef struct
{
	uint64_t SequenceID;
	uint8_t Slot;
	TPlayerActionType Type;
	uint32_t Data;
} TPlayerAction;

class TGameLogic
{
private:
	TGameData* Data;
	TLogicListener* Listener;
	TGameTime CurrentTime;
	std::vector<TPlayerAction> PlayerActions;
	std::mutex PlayerActionsMutex;

	float MovePlayer(TPlayer* Player, TPlayerDirection Direction, float Distance, bool Recurse = true); // updates a player's position in a certain direction for a certain distance
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
		
	void ApplyPlayerActionToData(const TPlayerAction PlayerAction); // updates the data based on a player action

public:
	uint64_t SequenceID; // last ID value sent to the server
	uint64_t ServerSequenceID; // ID value of last full server update
	uint64_t LastProcessedSequenceID; // last sequence ID that was processed

	TGameLogic(TGameData* SetData, TLogicListener* SetListener);
	void Process(TGameTime Delta);

	void AddPlayerAction(uint64_t SequenceID, uint8_t Slot, TPlayerActionType Type, uint32_t Data); // adds a new player action to the list to be processed.
};

class TClientLogic: public TLogicListener
{
private:
	TGameData* Data;	
private:
	void LogicBombExploding(const TFieldPosition& FieldPosition) override;
	void LogicBombExploded(const TFieldPosition& FieldPosition) override;
	void LogicPlayerDying(uint8_t Slot) override;
	void LogicPlayerDied(uint8_t Slot) override;
	void LogicRoundEnded() override;
public:
	TGameLogic GameLogic;

	TClientLogic(TGameData* SetData);
	void Process(TGameTime Delta);
};

class TAnimation
{
private:
public:
	intptr_t ID;
	TGameTime Time;
	TAnimation() : ID(0), Time(0) {};
	void Process(TGameTime Delta) { Time += Delta; };
};