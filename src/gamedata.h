#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <vector>
#include <list>
#include <SFML/Graphics.hpp>

const int INVALID_SLOT = 255;
const int MAX_NUM_SLOTS = 10;

const int MAX_NUM_ROUNDS = 50;

typedef uint8_t TGameStatus;

const TGameStatus GAME_NONE = 0;
const TGameStatus GAME_INLOBBY = 1;
const TGameStatus GAME_MATCHSTARTED = 2;
const TGameStatus GAME_ROUNDSTARTING = 3;
const TGameStatus GAME_PLAYING = 4;
const TGameStatus GAME_ROUNDENDED = 5;
const TGameStatus GAME_MATCHENDED = 6;

typedef struct 
{
	float X;
	float Y;
} TPlayerPosition;

typedef struct 
{
	uint8_t X;
	uint8_t Y;
} TFieldPosition;

typedef uint32_t TGameTime;

typedef uint8_t TPlayerDirection;

const TPlayerDirection DIRECTION_NONE = 0;
const TPlayerDirection DIRECTION_LEFT = 1 << 0;
const TPlayerDirection DIRECTION_RIGHT = 1 << 1;
const TPlayerDirection DIRECTION_UP = 1 << 2;
const TPlayerDirection DIRECTION_DOWN = 1 << 3;

typedef int16_t TPlayerSpeed;

const TPlayerSpeed SPEED_NOTMOVING = 0;
const TPlayerSpeed SPEED_NORMAL = 1000; // just a random value that has room for slower and faster speeds

typedef uint8_t TPlayerState;

const TPlayerState PLAYER_NOTPLAYING = -1;
const TPlayerState PLAYER_ALIVE = 0;
const TPlayerState PLAYER_DYING = 1;
const TPlayerState PLAYER_DEAD = 2;

typedef struct 
{
	std::string Name;
	TPlayerPosition Position;
	TPlayerDirection Direction; // combination of one or more DIRECTION_ values
	TPlayerSpeed Speed;
	TPlayerState State;
	uint32_t RoundsWon; // number of rounds won by the player so far
	uint32_t Ranking; // ranking in the current round
	uint8_t MaxActiveBombs; // the maximum amount of bombs a player can drop at any time
	uint8_t BombRange; // the range of newly dropped bombs
	uint8_t ActiveBombs; // the number of unexploded bombs the player has dropped
	TGameTime TimeUntilNextState; // time in milliseconds until the player finishes dying (or other state changes)
	TGameTime TimeOfDeath; // time of death in the current round
} TPlayer;

typedef uint8_t TBombState;

const TBombState BOMB_NONE = 0;
const TBombState BOMB_TICKING = 1;
const TBombState BOMB_EXPLODING = 2;

typedef struct 
{
	TBombState State;
	TGameTime TimeUntilNextState; // time in milliseconds until the explosion or the end of the explosion
	uint8_t Range; // range of the explosion in fields
	uint8_t DroppedByPlayer; // slot of the player who dropped the bomb
} TBomb;

typedef uint8_t TFieldType;

const TFieldType FIELD_EMPTY = 0;
const TFieldType FIELD_BRICK = 1;
const TFieldType FIELD_SOLID = 2;

typedef struct 
{
	TFieldType Type;
	TFieldPosition Position;
	TBomb Bomb;
} TField;

typedef struct
{
	int X;
	int Y;
	bool Primary;
	bool Taken;
} TStartPosition;

class TArena
{
private:
	size_t Stride; // the length of one horizontal line (i.e. the width), used to determine the one-dimensional position	
	std::vector<TField*> Fields;

	void CreateFields(size_t StartIndex = 0);
	void ClearFields();
	bool ProcessSchemeLine(const std::string& Line);
	void TokenizeSchemeLine(const std::string& Line, std::string& Command, std::list<std::string>& Parameters);

public:
	std::string OriginalFileName;
	std::string Caption;
	size_t Width, Height;
	int BrickDensity;
	std::vector<TStartPosition> StartPositions;

	TArena();
	~TArena();

	void Clear();

	// set the dimensions of the arena
	void SetSize(uint8_t SetWidth, uint8_t SetHeight); // set the size and reset all fields to default values
	void AddRow(); // use while loading field info row by row

	void AddStartPosition(int PlayerIndex, int X, int Y, bool IsPrimary);

	// retrieve a field at a position
	void At(uint8_t X, uint8_t Y, TField*& Field); // returns pointer to field strunct in Field
	void At(const TFieldPosition Position, TField*& Field); // returns pointer to field strunct in Field
	const TField& At(uint8_t X, uint8_t Y);
	const TField& At(const TFieldPosition Position);

	bool LoadFromFile(const std::string& FileName, int FileType = 0);
};

class TGameData
{
private:
	void ApplyBrickDensity();
	void PositionPlayers();
	void ClearMapFieldsForPlayer(int X, int Y);
public:
	std::string GameName;
	TGameTime MaxRunTime;
	uint32_t MaxRounds;
	uint32_t CurrentRound;
	TGameStatus Status;
	TPlayer Players[MAX_NUM_SLOTS];
	TArena Arena;
	TGameTime CurrentTime;
	sf::Color PlayerColors[MAX_NUM_SLOTS];

	TGameData();

	void Reset();
	void InitNewGame();
	void InitNewRound();

	void SetName(const std::string& SetName);
	bool AddPlayer(const std::string& SetName, int SetSlot = INVALID_SLOT);
	bool RemovePlayer(int Slot);
	bool SetPlayerName(int Slot, const std::string& SetName);

	bool BombInField(uint8_t X, uint8_t Y, bool OnlyUnexploded);

	void UpdateGameFrom(TGameData* Source);
};