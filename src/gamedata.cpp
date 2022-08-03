#include "gamedata.h"

// --- TArena ---

TArena::TArena()
:	Fields(),
	Stride(0),
	Width(0),
	Height(0)
{
}

TArena::~TArena()
{
	Clear();
}

TField* TArena::At(uint8_t X, uint8_t Y)
{
	size_t index = Stride * (size_t)Y + X;
	return Fields.at(index);
}

TField* TArena::At(const TFieldPosition Position)
{
	size_t index = Stride * (size_t)Position.Y + Position.X;
	return Fields.at(index);
}

void TArena::SetSize(uint8_t SetWidth, uint8_t SetHeight)
{
	Clear();

	Width = SetWidth;
	Height = SetHeight;
	Stride = Width;

	Fields.resize((size_t)Width * (size_t)Height);

	CreateFields();
}

void TArena::CreateFields()
{
	for (int i = 0; i < Fields.size(); i++)
	{
		TField* field = new TField();

		field->Type = FIELD_EMPTY;
		field->Position.X = i % (uint8_t)Stride;
		field->Position.Y = i / (uint8_t)Stride;

		Fields[i] = field;
	}
}

void TArena::Clear()
{
	for (auto it = Fields.begin(); it != Fields.end(); it++)
		delete (*it);
	Fields.clear();
};

// --- TGameData ---

TGameData::TGameData()
:	GameName(),
	MaxRunTime(0),
	MaxRounds(1),
	Status(GAME_NONE),
	Players(),
	Bombs(),
	Arena()
{
	for (int i = 0; i < MAX_NUM_SLOTS; i++)
		Players[i].State = PLAYER_NOTPLAYING;
}

void TGameData::Reset()
{
	Status = GAME_NONE;
	for (int i = 0; i < MAX_NUM_SLOTS; i++)
		Players[i].State = PLAYER_NOTPLAYING;
}

bool TGameData::AddPlayer(const std::string& SetName, int SetSlot)
{
	// check if the slot is available
	if (SetSlot != INVALID_SLOT && SetSlot <= MAX_NUM_SLOTS)
	{
		if (Players[SetSlot].State != PLAYER_NOTPLAYING)
			SetSlot = -1;
	}
	// find a free slot
	else
		for (int i = 0; i < MAX_NUM_SLOTS; i++)
			if (Players[i].State == PLAYER_NOTPLAYING)
			{
				SetSlot = i;
				break;
			}
	// exit if there are no slots
	if (SetSlot < 0)
		return false;

	// initialize player
	Players[SetSlot].State = PLAYER_ALIVE;
	Players[SetSlot].Name = SetName;

	return true;
}