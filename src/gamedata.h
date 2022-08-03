#pragma once

#include <cstdint>
#include <vector>
#include <string>

const int INVALID_SLOT = 255;
const int MAX_NUM_SLOTS = 8;

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
const TFieldType FIELD_CRUMBLEWALL = 1;
const TFieldType FIELD_SOLIDWALL = 2;

typedef struct 
{
	TFieldType Type;
	TFieldPosition Position;
} TField;

class TArena
{
private:
	size_t Stride; // the length of one horizontal line (i.e. the width), used to determine the one-dimensional position	
	std::vector<TField*> Fields;
	void CreateFields();
	void Clear();
public:
	size_t Width, Height;

	TArena();
	~TArena();

	// set the dimensions of the arena
	void SetSize(uint8_t SetWidth, uint8_t SetHeight); 

	// retrieve a field at a position (read-only)
	TField* At(uint8_t X, uint8_t Y);
	TField* At(const TFieldPosition Position);

	// TODO: add methods to update fields
};

class TGameData
{
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
};