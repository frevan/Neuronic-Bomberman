#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <vector>
#include <list>

const int INVALID_SLOT = 255;
const int MAX_NUM_SLOTS = 10;

typedef uint8_t TGameStatus;

const TGameStatus GAME_NONE = 0;
const TGameStatus GAME_INLOBBY = 1;
const TGameStatus GAME_STARTING = 2;
const TGameStatus GAME_RUNNING = 3;
const TGameStatus GAME_ENDED = 4;

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

const TPlayerDirection DIRECTION_UP = 1 << 0;
const TPlayerDirection DIRECTION_DOWN = 1 << 1;
const TPlayerDirection DIRECTION_LEFT = 1 << 2;
const TPlayerDirection DIRECTION_RIGHT = 1 << 3;

typedef int16_t TPlayerSpeed;

const TPlayerSpeed SPEED_NOTMOVING = 0;
const TPlayerSpeed SPEED_NORMAL = 256;

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
} TPlayer;

typedef uint8_t TBombState;

const TBombState BOMB_TICKING = 0;
const TBombState BOMB_EXPLODING = 1;
const TBombState BOMB_EXPLODED = 2;

typedef struct 
{
	TFieldPosition Position;
	TGameTime TimeUntilExplosion;
	TBombState State;
} TBomb;

typedef uint8_t TFieldType;

const TFieldType FIELD_EMPTY = 0;
const TFieldType FIELD_BRICK = 1;
const TFieldType FIELD_SOLID = 2;

typedef struct 
{
	TFieldType Type;
	TFieldPosition Position;
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
public:
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

	// retrieve a field at a position (read-only)
	TField* At(uint8_t X, uint8_t Y);
	TField* At(const TFieldPosition Position);
};

class TGameData
{
private:
	bool ProcessSchemeLine(const std::string& Line);
	void TokenizeSchemeLine(const std::string& Line, std::string& Command, std::list<std::string>& Parameters);
	void ApplyBrickDensity();
	void PositionPlayers();
	void ClearMapFieldsForPlayer(int X, int Y);
public:
	std::string GameName;
	TGameTime MaxRunTime;
	uint32_t MaxRounds;
	TGameStatus Status;
	TPlayer Players[MAX_NUM_SLOTS];
	std::vector<TBomb> Bombs;
	TArena Arena;

	TGameData();

	void Reset();
	bool AddPlayer(const std::string& SetName, int SetSlot = INVALID_SLOT);
	bool LoadMapFromFile(const std::string& FileName, int FileType = 0);
	void InitNewGame();
};