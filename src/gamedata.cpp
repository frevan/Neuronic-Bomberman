#include "gamedata.h"

#include <fstream>
#include <string>
#include <list>

#include "utility/stringutil.h"

const int MaxRowCountConst = 255;

// --- TArena ---

TArena::TArena()
:	Fields(),
	Stride(0),
	Width(0),
	Height(0),
	OriginalFileName(),
	Caption(),
	BrickDensity(0),
	StartPositions()
{
}

TArena::~TArena()
{
	Clear();
}

void TArena::At(uint8_t X, uint8_t Y, TField*& Field)
{
	size_t index = Stride * (size_t)Y + X;
	Field = Fields.at(index);
}

void TArena::At(const TFieldPosition Position, TField*& Field)
{
	size_t index = Stride * (size_t)Position.Y + Position.X;
	Field = Fields.at(index);
}

const TField& TArena::At(uint8_t X, uint8_t Y)
{
	TField* field{};
	At(X, Y, field);
	return (*field);
}

const TField& TArena::At(const TFieldPosition Position)
{
	TField* field{};
	At(Position, field);
	return (*field);
}

void TArena::SetSize(uint8_t SetWidth, uint8_t SetHeight)
{
	ClearFields();

	Width = SetWidth;
	Height = SetHeight;
	Stride = Width;

	Fields.resize((size_t)Width * (size_t)Height);

	CreateFields();
}

void TArena::AddRow()
{
	size_t oldHeight = Height;
	Height++;

	Fields.resize(Width * Height);

	CreateFields(Width * oldHeight); // add and init the new fields
}

void TArena::CreateFields(size_t StartIndex)
{
	for (size_t i = StartIndex; i < Fields.size(); i++)
	{
		TField* field = new TField();

		field->Type = FIELD_EMPTY;
		field->Position.X = (uint8_t)(i % Stride);
		field->Position.Y = (uint8_t)(i / Stride);

		Fields[i] = field;
	}
}

void TArena::Clear()
{
	OriginalFileName = "";
	Caption = "";
	Width = 0;
	Height = 0;
	BrickDensity = 0;

	ClearFields();

	StartPositions.clear();
};

void TArena::ClearFields()
{
	for (auto it = Fields.begin(); it != Fields.end(); it++)
		delete (*it);
	Fields.clear();
}

void TArena::AddStartPosition(int PlayerIndex, int X, int Y, bool IsPrimary)
{
	// resize container if necessary
	if (StartPositions.size() <= PlayerIndex)
		StartPositions.resize((size_t)PlayerIndex + 1);

	// set item
	StartPositions[PlayerIndex].X = X;
	StartPositions[PlayerIndex].Y = Y;
	StartPositions[PlayerIndex].Primary = IsPrimary;
}

bool TArena::LoadFromFile(const std::string& FileName, int FileType)
{
	Clear();

	std::string ext;

	bool success = true;

	// .sch file from Atomic Bomberman
	if (FileType == 0)
	{
		std::string temp;
		std::ifstream ifs(FileName);
		while (getline(ifs, temp))
		{
			if (!ProcessSchemeLine(temp))
			{
				success = false;
				break;
			}
		}
	}

	if (success)
		OriginalFileName = FileName;

	return success;
}

bool TArena::ProcessSchemeLine(const std::string& line)
{
	std::string localLine = line;

	trim(localLine);
	if (localLine.empty())
		return true;

	// ignore comments (start with ";") and anything that's not a command (starts with "-")
	std::string::iterator c = localLine.begin();
	if (*c == ';' || *c != '-')
		return true;

	// extract command and parameters
	std::string cmd;
	std::list<std::string> params;
	TokenizeSchemeLine(localLine, cmd, params);

	// process command
	// --> version
	if (cmd == "-v")
	{
		if (params.size() != 1)
			return false;
		std::string p1 = *(params.begin());
		int d = atoi(p1.c_str());
		if (d != 2)
			return false;
	}
	// --> name (caption)
	else if (cmd == "-n")
	{
		if (params.size() != 1)
			return false;
		else
			Caption = *(params.begin());
	}
	// --> brick density
	else if (cmd == "-b")
	{
		if (params.size() != 1)
			return false;
		else
		{
			std::string p1 = *(params.begin());
			int d = atoi(p1.c_str());
			if (d < 0)
				BrickDensity = 0;
			else if (d > 100)
				BrickDensity = 100;
			else
				BrickDensity = d;
		}
	}
	// --> fields
	else if (cmd == "-r")
	{
		if (params.size() < 2)
			return false;
		else
		{
			// actual array data (# is solid, : is brick, . is blank)
			std::list<std::string>::iterator it = params.begin();
			std::string p[4];
			for (int i = 0; i < 2; i++)
				p[i] = *it++;
			size_t newColCount = 0, newRowCount = 0;
			size_t rownumber = atoi(p[0].c_str());
			if (rownumber >= 0 && rownumber < MaxRowCountConst)
			{
				newRowCount = rownumber >= newRowCount ? rownumber + 1 : newRowCount;
				newColCount = p[1].size() > newColCount ? p[1].size() : newColCount;
				if (Width == 0)
					SetSize((uint8_t)newColCount, (uint8_t)newRowCount);
				else
					AddRow();

				std::string::iterator colIt;
				int idx = 0;
				for (colIt = p[1].begin(); colIt != p[1].end(); ++colIt)
				{
					if (idx >= newColCount)
						break;
					char c = *colIt;

					TField* field{};
					At(idx, (uint8_t)rownumber, field);
					switch (c)
					{
						case '.': field->Type = FIELD_EMPTY; break;
						case ':': field->Type = FIELD_BRICK; break;
						case '#': field->Type = FIELD_SOLID; break;
					}
					idx++;
				}
			}
		}
	}
	// --> player start locations
	else if (cmd == "-s")
	{
		if (params.size() < 3)
			return false;
		else
		{
			// actual array data
			std::list<std::string>::iterator it = params.begin();
			std::string p[4];
			int idx = 0;
			while (idx < params.size() && idx < 4)
			{
				p[idx] = *it++;
				idx++;
			}
			if (idx < 4)		// some schemes don't include the "primary" field here
				p[3] = "1";
			int playeridx = atoi(p[0].c_str());
			int x = atoi(p[1].c_str());
			int y = atoi(p[2].c_str());
			bool primary = atoi(p[3].c_str()) != 0;
			AddStartPosition(playeridx, x, y, primary);
		}
	}
	// --> powerup info
	else if (cmd == "-p")
	{
		/*
		if (params.size() < 5)
			return false;
		else
		{
			// actual array data
			list<string>::iterator it = params.begin();
			string p[5];
			for (int i = 0; i < 5; i++)
				p[i] = *it++;
			int idx = atoi(p[0].c_str());
			if (idx >= 0 && idx < MaxPowerupCountConst)
			{
				ensurePowerups(idx+1);		// ensure storage
				powerups[idx].bornwith = (atoi(p[1].c_str()) != 0);
				powerups[idx].hasOverride = (atoi(p[2].c_str()) != 0);
				powerups[idx].overrideValue = atoi(p[3].c_str());
				powerups[idx].forbidden = (atoi(p[4].c_str()) != 0);
			}
		}
		*/
	}
	else
		return false;

	return (!cmd.empty());
}

void TArena::TokenizeSchemeLine(const std::string& Line, std::string& Command, std::list<std::string>& Parameters)
{
	std::string tempString;

	size_t offset = 0;
	size_t pos = Line.find_first_of(",", offset);
	while (pos != std::string::npos)
	{
		if (Command.empty())
		{
			Command = Line.substr(offset, pos - offset);
			std::transform(Command.begin(), Command.end(), Command.begin(), ::tolower);
		}
		else
		{
			tempString = Line.substr(offset, pos - offset);
			Parameters.push_back(tempString);
		}

		offset = pos + 1;
		pos = Line.find_first_of(",", offset);
	}

	if (offset < Line.size())
	{
		tempString = Line.substr(offset, Line.size() - offset);
		Parameters.push_back(tempString);
	}
}

// --- TGameData ---

TGameData::TGameData()
:	GameName(),
	MaxRunTime(0),
	MaxRounds(1),
	Status(GAME_NONE),
	Players(),
	Arena()
{
	for (int i = 0; i < MAX_NUM_SLOTS; i++)
		Players[i].State = PLAYER_NOTPLAYING;

	PlayerColors[0] = sf::Color::Blue;
	PlayerColors[1] = sf::Color::Magenta;
	PlayerColors[2] = sf::Color::Yellow;
	PlayerColors[3] = sf::Color(160, 120, 80);
	PlayerColors[4] = sf::Color(80, 80, 0);
	PlayerColors[5] = sf::Color(80, 0, 80);
	PlayerColors[6] = sf::Color(0, 80, 80);
	PlayerColors[7] = sf::Color(80, 80, 80);
	PlayerColors[8] = sf::Color(160, 160, 160);
	PlayerColors[9] = sf::Color(160, 0, 160);
}

void TGameData::Reset()
{
	Status = GAME_NONE;

	GameName = "Bombi";
	MaxRounds = 1;

	for (int i = 0; i < MAX_NUM_SLOTS; i++)
		Players[i].State = PLAYER_NOTPLAYING;
}

bool TGameData::AddPlayer(const std::string& SetName, bool SetOwned, uint8_t SetSlot)
{
	// check if the slot is available
	if (SetSlot != INVALID_SLOT && SetSlot <= MAX_NUM_SLOTS)
	{
		if (Players[SetSlot].State != PLAYER_NOTPLAYING)
			SetSlot = -1;
	}
	// find a free slot
	else
	{
		for (int i = 0; i < MAX_NUM_SLOTS; i++)
			if (Players[i].State == PLAYER_NOTPLAYING)
			{
				SetSlot = i;
				break;
			}
	}
	// exit if there are no slots
	if (SetSlot == INVALID_SLOT || SetSlot < 0 || SetSlot >= MAX_NUM_SLOTS)
		return false;

	// initialize player
	Players[SetSlot].State = PLAYER_ALIVE;
	Players[SetSlot].Name = SetName;
	Players[SetSlot].Owned = SetOwned;

	return true;
}

bool TGameData::RemovePlayer(int Slot)
{
	bool result = false;
	if (Slot >= 0 && Slot < MAX_NUM_SLOTS)
		if (Players[Slot].State != PLAYER_NOTPLAYING)
		{
			result = true;
			Players[Slot].State = PLAYER_NOTPLAYING;
		}

	return result;
}

void TGameData::InitNewGame()
{
	CurrentTime = 0;
	CurrentRound = 0;

	for (int idx = 0; idx < MAX_NUM_SLOTS; idx++)
		Players[idx].RoundsWon = 0;

	//InitNewRound();
}

void TGameData::InitNewRound()
{
	CurrentTime = 0;
	CurrentRound++;

	for (int idx = 0; idx < MAX_NUM_SLOTS; idx++)
	{
		TPlayer* p = &Players[idx];
		if (p->State == PLAYER_NOTPLAYING)
			continue;
		
		p->Speed = 0;
		p->Ranking = 0;
		p->MaxActiveBombs = 1;
		p->BombRange = 1;
		p->ActiveBombs = 0;
		p->TimeOfDeath = 0;
		p->State = PLAYER_ALIVE;		
	}

	for (int x = 0; x < Arena.Width; x++)
		for (int y = 0; y < Arena.Height; y++)
		{
			TField* field{};
			Arena.At(x, y, field);
			field->Bomb.State = BOMB_NONE;
		}

	ApplyBrickDensity();
	PositionPlayers();
}

void TGameData::ApplyBrickDensity()
{
	// count bricks
	int brickCount = 0;
	for (int i = 0; i < Arena.Width; i++)	
		for (int j = 0; j < Arena.Height; j++)
		{
			if (Arena.At(i, j).Type == FIELD_BRICK)
				brickCount++;
		}

	// remove them
	if (Arena.BrickDensity < 100)
	{
		srand((unsigned int)time(nullptr));
		int bricksToLose = (int)(brickCount - (brickCount / 100.0 * Arena.BrickDensity));
		for (int i = 0; i < Arena.Width; i++)	
			for (int j = 0; j < Arena.Height; j++)
			{
				TField* field{};
				Arena.At(i, j, field);
				if (field->Type != FIELD_BRICK)
					continue;

				bool keep = rand() % brickCount < Arena.BrickDensity;
				if (!keep)
				{
					field->Type = FIELD_EMPTY;
					bricksToLose--;
					if (bricksToLose <= 0)
						break;
				}
			}
	}
}

void TGameData::PositionPlayers()
{
	if (Arena.StartPositions.size() == 0)
		return;

	for (int i = 0; i < Arena.StartPositions.size(); i++)
		Arena.StartPositions[i].Taken = false;

	int startpos_idx = rand() % Arena.StartPositions.size(); // start at a "random" position
	for (int it = 0; it < MAX_NUM_SLOTS; it++)
	{
		if (Players[it].State == PLAYER_NOTPLAYING)
			continue;

		Players[it].Position.X = 0.f;
		Players[it].Position.Y = 0.f;

		bool found = false;
		// -> first try the position of the player's slot
		if (Arena.StartPositions.size() >= it)
		{
			if (!Arena.StartPositions[it].Taken)
			{
				Players[it].Position.X = Arena.StartPositions[it].X + 0.5f;
				Players[it].Position.Y = Arena.StartPositions[it].Y + 0.5f;
				Arena.StartPositions[it].Taken = true;
				found = true;
			}
		}
		// --> if not available, try primary positions
		if (!found)
		{
			for (int i = 0; i < Arena.StartPositions.size(); i++)
			{
				int idx = (startpos_idx + i) % Arena.StartPositions.size();
				if (Arena.StartPositions[idx].Taken)
					continue;
				if (!Arena.StartPositions[idx].Primary)
					continue;
				Players[it].Position.X = Arena.StartPositions[idx].X + 0.5f;
				Players[it].Position.Y = Arena.StartPositions[idx].Y + 0.5f;
				Arena.StartPositions[idx].Taken = true;
				found = true;
				break;
			}
		}
		// --> then try secondary positions
		if (!found)	
			for (int i = 0; i < Arena.StartPositions.size(); i++)
			{
				int idx = (startpos_idx + i) % Arena.StartPositions.size();
				if (Arena.StartPositions[idx].Taken)
					continue;
				Players[it].Position.X = Arena.StartPositions[idx].X + 0.5f;
				Players[it].Position.Y = Arena.StartPositions[idx].Y + 0.5f;
				Arena.StartPositions[idx].Taken = true;
				break;
			}

		// clear fields around player, if necessary, in a + shape
		int fieldx = (int)Players[it].Position.X;
		int fieldy = (int)Players[it].Position.Y;
		ClearMapFieldsForPlayer(fieldx, fieldy);
	}
}

void TGameData::ClearMapFieldsForPlayer(int X, int Y)
{
	TField* field{};
	Arena.At(X, Y, field);

	// player's own field
	if (field->Type == FIELD_BRICK)
		field->Type = FIELD_EMPTY;

	// x-1	
	if (X > 0)
	{
		Arena.At(X - 1, Y, field);
		if (field->Type == FIELD_BRICK)
			field->Type = FIELD_EMPTY;
	}

	// x+1
	if (X < Arena.Width - 1)
	{
		Arena.At(X + 1, Y, field);
		if (field->Type == FIELD_BRICK)
			field->Type = FIELD_EMPTY;
	}

	// y-1
	if (Y > 0)
	{
		Arena.At(X, Y - 1, field);
		if (field->Type == FIELD_BRICK)
			field->Type = FIELD_EMPTY;
	}

	// y+1
	if (Y < Arena.Height - 1)
	{
		Arena.At(X, Y + 1, field);
		if (field->Type == FIELD_BRICK)
			field->Type = FIELD_EMPTY;
	}
}

bool TGameData::BombInField(uint8_t X, uint8_t Y, bool OnlyUnexploded)
{
	TField* field{};
	Arena.At(X, Y, field);

	if (field->Bomb.State == BOMB_NONE)
		return false;
	else if (OnlyUnexploded && field->Bomb.State == BOMB_EXPLODING)
		return false;
	else
		return true;
}

bool TGameData::SetPlayerName(int Slot, const std::string& SetName)
{
	bool result = false;
	if (Slot >= 0 && Slot < MAX_NUM_SLOTS)
		if (Players[Slot].State != PLAYER_NOTPLAYING)
		{
			result = true;
			Players[Slot].Name = SetName;
		}

	return result;
}

void TGameData::SetName(const std::string& SetName)
{
	GameName = SetName;
}

void TGameData::UpdateGameFrom(TGameData* Source)
{
	for (uint8_t slot = 0; slot < MAX_NUM_SLOTS; slot++)
	{
		TPlayer* p = &Players[slot];
		const TPlayer* src = &Source->Players[slot];

		p->Position = src->Position;
		p->Direction = src->Direction;
		p->Speed = src->Speed;
		p->State = src->State;
		p->RoundsWon = src->RoundsWon;
		p->Ranking = src->Ranking;
		p->MaxActiveBombs = src->MaxActiveBombs;
		p->BombRange = src->BombRange;
		p->ActiveBombs = src->ActiveBombs;
		p->TimeUntilNextState = src->TimeUntilNextState;
		p->TimeOfDeath = src->TimeOfDeath;
	}

	for (uint8_t x = 0; x < Arena.Width; x++)
		for (uint8_t y = 0; y < Arena.Height; y++)
		{
			TField* field{};
			Arena.At(x, y, field);
			TField src = Source->Arena.At(x, y);

			field->Type = src.Type;
			field->Bomb.DroppedByPlayer = src.Bomb.DroppedByPlayer;
			field->Bomb.Range = src.Bomb.Range;
			field->Bomb.State = src.Bomb.State;
			field->Bomb.TimeUntilNextState = src.Bomb.TimeUntilNextState;
		}

	CurrentTime = Source->CurrentTime;
}

/*
void TGameData::UpdateGameFrom(uint32_t FrameIndex, TFullMatchUpdateInfo* Info)
{
	for (uint8_t slot = 0; slot < MAX_NUM_SLOTS; slot++)
	{
		TPlayer* p = &Players[slot];

		p->Position = Info->PlayerPositions[slot];
		p->Direction = Info->PlayerDirections[slot];
	}

	for (uint8_t y = 0; y < Arena.Height; y++)
		for (uint8_t x = 0; x < Arena.Width; x++)		
		{
			TFieldType type = Info->FieldTypes[y * Arena.Width + x];

			TField* field{};
			Arena.At(x, y, field);
			field->Type = type;
		}
}

// --- 	TFullMatchUpdateInfo ---
TFullMatchUpdateInfo::TFullMatchUpdateInfo(size_t ArenaWidth, size_t ArenaHeight)
:	PlayerDirections(),
	PlayerPositions()
{
	FieldTypes = (TFieldType*)malloc(ArenaWidth * ArenaHeight * sizeof(TFieldType));
}

TFullMatchUpdateInfo::~TFullMatchUpdateInfo()
{
	free(FieldTypes);
}
*/