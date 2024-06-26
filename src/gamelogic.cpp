#include "gamelogic.h"

#include <cassert>

const float SmallestDistanceConst = 0.000001f;
const float CornerAnimationIncConst = 0.05f;

// TGameLogic

TGameLogic::TGameLogic(TGameData* SetData, TLogicListener* SetListener)
:	Data(SetData),
	Listener(SetListener),
	CurrentTime(0),
	PlayerActions(),
	PlayerActionsMutex(),
	SequenceID(1),
	ServerSequenceID(0),
	LastProcessedSequenceID(0)
{
}

void TGameLogic::Process(TGameTime Delta)
{
	if (Data->Status != GAME_PLAYING)
		return;

	// apply actions to data
	{
		std::lock_guard<std::mutex> g(PlayerActionsMutex);
		for (auto it = PlayerActions.begin(); it != PlayerActions.end(); it++)
		{
			ApplyPlayerActionToData(*it);
			LastProcessedSequenceID = (*it).SequenceID;
		}
		PlayerActions.clear();
	}

	// calculate updates
	UpdatePlayerPositions(Delta);
	UpdateBombs(Delta);
	CheckForExplodedPlayers();
	UpdateDyingPlayers(Delta);

	// move time forward
	CurrentTime += Delta;
	Data->CurrentTime = CurrentTime;
}

void TGameLogic::ApplyPlayerActionToData(const TPlayerAction PlayerAction)
{
	TPlayer* p = &(Data->Players[PlayerAction.Slot]);	

	if (PlayerAction.Type == actionChangeDirection)
	{
		if (p->State == PLAYER_NOTPLAYING)
			return;

		p->Direction = PlayerAction.Data;
	}
	else if (PlayerAction.Type == actionDropBomb)
	{
		if (p->State == PLAYER_NOTPLAYING)
			return;
		if (p->ActiveBombs == p->MaxActiveBombs)
			return;

		// determine the exact position where to drop it (top left pos of the field)
		TFieldPosition pos;
		pos.X = static_cast<int>(trunc(p->Position.X));
		pos.Y = static_cast<int>(trunc(p->Position.Y));

		// check if there's already a bomb at this position
		if (Data->BombInField(pos.X, pos.Y, false))
			return;

		// add the new bomb
		TField* field{};
		Data->Arena.At(pos, field);
		if (!field)
			return;

		field->Bomb.State = BOMB_TICKING;
		field->Bomb.DroppedByPlayer = PlayerAction.Slot;
		field->Bomb.TimeUntilNextState = 2000; // 2 seconds
		field->Bomb.Range = p->BombRange;

		// update the player
		p->ActiveBombs++;
	}

	p->LastProcessedSequenceID = PlayerAction.SequenceID;
}

void TGameLogic::AddPlayerAction(uint64_t SequenceID, uint8_t Slot, TPlayerActionType Type, uint32_t Data)
{
	// create new item
	TPlayerAction item { SequenceID, Slot, Type, Data };

	std::lock_guard<std::mutex> g(PlayerActionsMutex);

	// add to the list
	PlayerActions.push_back(item);
}

float TGameLogic::MovePlayer(TPlayer* Player, TPlayerDirection Direction, float Distance, bool Recurse)
{
	const float stepsize = 0.05f;
	float distance_to_go = Distance;
	float distance_moved = 0;
	TPlayerDirection curdirection = Direction;

	float curposx = Player->Position.X, curposy = Player->Position.Y;
	int factor = 1;

	// LOOP
	while (distance_to_go > SmallestDistanceConst)
	{
		// try moving in the requested direction
		float actualstep = distance_to_go > stepsize ? stepsize : distance_to_go;
		TPlayerDirection otherdirection;
		float alloweddistance = CanMove(Player, curdirection, actualstep, otherdirection);
		if (alloweddistance)
		{
			// --> move
			AdjustPlayerPos(Player, curdirection, alloweddistance);
			// --> adjust variables
			distance_to_go -= alloweddistance;
			distance_moved += alloweddistance;
			// --> center position in non-moving axis
			float diff, v;
			int fieldx, fieldy;
			if (curdirection == Direction) 
				switch (curdirection)
				{
					case DIRECTION_LEFT:
					case DIRECTION_RIGHT:
						fieldy = (int)Player->Position.Y;
						diff = (fieldy + 0.5f) - Player->Position.Y;
						if (diff)
						{
							v = (float)fabs(diff) < CornerAnimationIncConst ? (float)fabs(diff) : CornerAnimationIncConst;
							if (diff < 0)	
								Player->Position.Y = Player->Position.Y - v;
							else			
								Player->Position.Y = Player->Position.Y + v;
						}
						break;
					case DIRECTION_UP:
					case DIRECTION_DOWN:
						fieldx = (int)Player->Position.X;
						diff = (fieldx + 0.5f) - Player->Position.X;
						if (diff)
						{
							v = (float)fabs(diff) < CornerAnimationIncConst ? (float)fabs(diff) : CornerAnimationIncConst;
							if (diff < 0)	
								Player->Position.X = Player->Position.X - v;
							else			
								Player->Position.X = Player->Position.X + v;
						}
						break;
				};

			// --> end loop?
			if (distance_to_go < SmallestDistanceConst)
				break;
		}

		// restore primary direction
		if (curdirection != Direction)
		{
			curdirection = Direction;
			if (alloweddistance <= 0)
				distance_to_go = 0;
		}
		// try moving in another direction
		else if (otherdirection != DIRECTION_NONE)
		{
			//curdirection = otherdirection;
			float disttomove = actualstep - alloweddistance;
			AdjustPlayerPos(Player, otherdirection, disttomove);
			distance_to_go -= disttomove;
		}
		// no move left
		else if (alloweddistance < SmallestDistanceConst)
			distance_to_go = 0;
	}

	return distance_moved;
}

float TGameLogic::CanMove(TPlayer* Player, TPlayerDirection Direction, float Distance, TPlayerDirection& otherDirection)
{
	int curfield, nextfield, nextfieldx, nextfieldy;
	float curpos = 0.f, nextpos = 0.f;
	size_t mapsize = 0;
	otherDirection = DIRECTION_NONE;

	switch (Direction)
	{
		case DIRECTION_LEFT:
			curpos = Player->Position.X;
			nextpos = Player->Position.X - Distance;
			nextfieldx = (int)nextpos;
			nextfieldy = (int)Player->Position.Y;
			mapsize = Data->Arena.Width;
			break;
		case DIRECTION_RIGHT:
			curpos = Player->Position.X;
			nextpos = Player->Position.X + Distance;
			nextfieldx = (int)nextpos;
			nextfieldy = (int)Player->Position.Y;
			mapsize = Data->Arena.Width;
			break;
		case DIRECTION_UP:
			curpos = Player->Position.Y;
			nextpos = Player->Position.Y - Distance;
			nextfieldx = (int)Player->Position.X;
			nextfieldy = (int)nextpos;
			mapsize = Data->Arena.Height;
			break;
		case DIRECTION_DOWN:
			curpos = Player->Position.Y;
			nextpos = Player->Position.Y + Distance;
			nextfieldx = (int)Player->Position.X;
			nextfieldy = (int)nextpos;
			mapsize = Data->Arena.Height;
			break;
	}
	curfield = (int)curpos;
	nextfield = (int)nextpos;

	// check against map bounds
	if (nextpos < 0.5f)
		return curpos - 0.5f;
	else if (nextpos >= mapsize - 0.5f)
		return (mapsize - 0.5f) - curpos;

	// check the contents of the field	
	// --> if we stay in the same field	
	if (nextfield == curfield)
	{
		switch (Direction)
		{
			case DIRECTION_LEFT:
				if (nextpos - nextfield < 0.5)
					nextfieldx--;
				break;
			case DIRECTION_RIGHT:
				if (nextpos - nextfield > 0.5)
					nextfieldx++;
				break;
			case DIRECTION_UP:
				if (nextpos - nextfield < 0.5)
					nextfieldy--;
				break;
			case DIRECTION_DOWN:
				if (nextpos - nextfield > 0.5)
					nextfieldy++;
				break;
		}
	}
	// --> either way, check against the (new/old) target cell
	if (!CheckMoveAgainstCell(Player, nextfieldx, nextfieldy, Direction, otherDirection, true))
	{
		float dtr = (float)fabs(curfield + 0.5f - curpos);
		switch (Direction)
		{
			case DIRECTION_LEFT:
			case DIRECTION_UP:
				if ((float)fabs(curpos - curfield) < 0.5f)
					dtr = 0;
				break;
			case DIRECTION_RIGHT:
			case DIRECTION_DOWN:
				if ((float)fabs(curpos - curfield) > 0.5f)
					dtr = 0;
				break;
		}

		return dtr;
	}
	else
		return (float)fabs(nextpos - curpos);
}

void TGameLogic::AdjustPlayerPos(TPlayer* Player, TPlayerDirection Direction, float Distance)
{
	// --> move
	switch (Direction)
	{
		case DIRECTION_LEFT: Player->Position.X -= Distance; break;
		case DIRECTION_RIGHT: Player->Position.X += Distance; break;
		case DIRECTION_UP: Player->Position.Y -= Distance; break;
		case DIRECTION_DOWN: Player->Position.Y += Distance; break;
	}
}

bool TGameLogic::CheckMoveAgainstCell(TPlayer* Player, int CellX, int CellY, TPlayerDirection& Direction, TPlayerDirection& OtherDirection, bool Recurse)
{
	#ifdef NoBoundsMovement
	return true;
	#endif

	OtherDirection = DIRECTION_NONE;
	if (CellX < 0 || CellX >= Data->Arena.Width || CellY < 0 || CellY >= Data->Arena.Height)
		return false;

	// check cell contents to see if we can move there
	TField field = Data->Arena.At(CellX, CellY);
	bool canmove = true;
	if (field.Type == FIELD_BRICK || field.Type == FIELD_SOLID)
		canmove = false; 
	if (Data->BombInField(CellX, CellY, true))
	{
		TFieldPosition pos = CalculatePlayerField(Player);
		if (pos.X != CellX || pos.Y != CellY)
			canmove &= !Data->BombInField(CellX, CellY, true);
	}
	if (canmove)
		return true;

	// if not, try another direction for a while
	if (Recurse)
	{
		// --> determine potential direction
		int x = CellX, y = CellY;
		float temp;
		switch (Direction)
		{
			case DIRECTION_LEFT:
			case DIRECTION_RIGHT:
				temp = Player->Position.Y - (int)Player->Position.Y;
				if (temp > 0.5f)
					OtherDirection = DIRECTION_DOWN;
				else if (temp < 0.5f)	
					OtherDirection = DIRECTION_UP;
				y = OtherDirection == DIRECTION_UP ? CellY - 1 : CellY + 1;
				break;
			case DIRECTION_UP:
			case DIRECTION_DOWN:
				temp = Player->Position.X - (int)Player->Position.X;
				if (temp > 0.5f)			
					OtherDirection = DIRECTION_RIGHT;
				else if (temp < 0.5f)	
					OtherDirection = DIRECTION_LEFT;
				x = OtherDirection == DIRECTION_LEFT ? CellX - 1 : CellX + 1;
				break;
		}
		// --> check
		if (x >= 0 && x < Data->Arena.Width && y >= 0 && y < Data->Arena.Height)
		{
			TPlayerDirection tempdir;
			if (!CheckMoveAgainstCell(Player, x, y, OtherDirection, tempdir, false))
				OtherDirection = DIRECTION_NONE;
		}
	}

	return false;
}

void TGameLogic::UpdatePlayerPositions(TGameTime Delta)
{
	for (int i = 0; i < MAX_NUM_SLOTS; i++)
	{
		TPlayer* player = &Data->Players[i];

		if (player->State != PLAYER_ALIVE)
			continue;

		if (player->Direction == 0)
			player->Speed = SPEED_NOTMOVING;
		else
			player->Speed = SPEED_NORMAL;

		float normalizedSpeed = player->Speed / (float)SPEED_NORMAL;
		float distanceTravelled = normalizedSpeed * (float)Delta / 1000.f * 4.0f;

		bool changed = false;
		if (!changed)
			if ((player->Direction & DIRECTION_LEFT) == DIRECTION_LEFT)
				changed = (MovePlayer(player, DIRECTION_LEFT, distanceTravelled) > 0);
		if (!changed)
			if ((player->Direction & DIRECTION_RIGHT) == DIRECTION_RIGHT)
				changed = (MovePlayer(player, DIRECTION_RIGHT, distanceTravelled) > 0);
		if (!changed)
			if ((player->Direction & DIRECTION_UP) == DIRECTION_UP)
				changed = (MovePlayer(player, DIRECTION_UP, distanceTravelled) > 0);
		if (!changed)
			if ((player->Direction & DIRECTION_DOWN) == DIRECTION_DOWN)
				changed = (MovePlayer(player, DIRECTION_DOWN, distanceTravelled) > 0);
	}
}

void TGameLogic::UpdateBombs(TGameTime Delta)
{
	for (uint8_t x = 0; x < Data->Arena.Width; x++)
		for (uint8_t y = 0; y < Data->Arena.Height; y++)
		{
			TField* field{};
			Data->Arena.At(x, y, field);

			if (field->Bomb.State == BOMB_TICKING)
			{
				if (field->Bomb.TimeUntilNextState <= Delta)
					ExplodeBomb(x, y);
				else
					field->Bomb.TimeUntilNextState -= Delta;
			}
			else if (field->Bomb.State == BOMB_EXPLODING)
			{
				if (field->Bomb.TimeUntilNextState <= Delta)
				{
					field->Bomb.State = BOMB_NONE;
					field->Bomb.DroppedByPlayer = INVALID_SLOT;
					if (Listener)
					{
						TFieldPosition position{x, y};
						Listener->LogicBombExploded(position);
					}
				}
				else
					field->Bomb.TimeUntilNextState -= Delta;
			}
		}
}

void TGameLogic::ExplodeBomb(uint8_t X, uint8_t Y)
{
	TField* field{};
	Data->Arena.At(X, Y, field);

	// update the player's active bomb count
	if (field->Bomb.DroppedByPlayer >= 0 && field->Bomb.DroppedByPlayer < MAX_NUM_SLOTS)
	{
		assert(Data->Players[field->Bomb.DroppedByPlayer].ActiveBombs > 0);
		Data->Players[field->Bomb.DroppedByPlayer].ActiveBombs--;
	}

	// explode the field itself
	field->Bomb.State = BOMB_EXPLODING;
	field->Bomb.TimeUntilNextState = 600; // explode for 0.6 seconds

	if (Listener)
		Listener->LogicBombExploding(field->Position);

	bool stopLeft = false, stopRight = false, stopUp = false, stopDown = false;

	for (int i = 1; i <= field->Bomb.Range; i++)
	{
		if (!stopLeft)
			ExplodeField(X - i, Y, field->Bomb, stopLeft);
		if (!stopRight)
			ExplodeField(X + i, Y, field->Bomb, stopRight);
		if (!stopUp)
			ExplodeField(X, Y - i, field->Bomb, stopUp);
		if (!stopDown)
			ExplodeField(X, Y + i, field->Bomb, stopDown);
	}
}

void TGameLogic::ExplodeField(uint8_t X, uint8_t Y, const TBomb& OriginalBomb, bool& Stop)
{
	if (X < 0 || X >= Data->Arena.Width || Y < 0 || Y >= Data->Arena.Height)
	{
		Stop = true;
		return;
	}

	TField* field{};
	Data->Arena.At(X, Y, field);
	
	if (field->Type == FIELD_SOLID)
	{
		Stop = true;
		return;
	}

	if (field->Type == FIELD_BRICK)
	{
		Stop = true;
		field->Type = FIELD_EMPTY;
	}

	if (field->Bomb.State == BOMB_TICKING && field->Bomb.DroppedByPlayer >= 0 && field->Bomb.DroppedByPlayer < MAX_NUM_SLOTS)
	{
		assert(Data->Players[field->Bomb.DroppedByPlayer].ActiveBombs > 0);
		Data->Players[field->Bomb.DroppedByPlayer].ActiveBombs--;
	}
	field->Bomb = OriginalBomb;
	field->Bomb.DroppedByPlayer = INVALID_SLOT;

	if (Listener)
		Listener->LogicBombExploding(field->Position);
}

void TGameLogic::CheckForExplodedPlayers()
{
	for (int idx = 0; idx < MAX_NUM_SLOTS; idx++)
	{
		TPlayer* p = &Data->Players[idx];
		if (p->State == PLAYER_NOTPLAYING || p->State == PLAYER_DEAD)
			continue;

		TFieldPosition playerPos = CalculatePlayerField(p);

		int leftField = static_cast<int>(trunc(p->Position.X - 0.49));
		int rightField = static_cast<int>(trunc(p->Position.X + 0.49));
		int topField = static_cast<int>(trunc(p->Position.Y - 0.49));
		int bottomField = static_cast<int>(trunc(p->Position.Y + 0.49));

		bool exploding = Data->Arena.At(playerPos.X, playerPos.Y).Bomb.State == BOMB_EXPLODING;
		if (leftField >= 0)
			exploding |= Data->Arena.At(leftField, playerPos.Y).Bomb.State == BOMB_EXPLODING;
		if (rightField < Data->Arena.Width)
			exploding |= Data->Arena.At(rightField, playerPos.Y).Bomb.State == BOMB_EXPLODING;
		if (topField >= 0)
			exploding |= Data->Arena.At(playerPos.X, topField).Bomb.State == BOMB_EXPLODING;
		if (bottomField < Data->Arena.Height)
			exploding |= Data->Arena.At(playerPos.X, bottomField).Bomb.State == BOMB_EXPLODING;

		if (exploding)
		{
			p->State = PLAYER_DYING;
			p->TimeUntilNextState = 1000;
			p->TimeOfDeath = Data->CurrentTime;

			if (Listener)
				Listener->LogicPlayerDying(idx);
		}
	}
}

void TGameLogic::UpdateDyingPlayers(TGameTime Delta)
{
	int alivePlayerCount = 0;

	for (int idx = 0; idx < MAX_NUM_SLOTS; idx++)
	{
		TPlayer* p = &Data->Players[idx];
		if (p->State == PLAYER_NOTPLAYING)
			continue;

		if (p->State == PLAYER_DYING)
		{
			if (p->TimeUntilNextState <= Delta)
			{
				p->State = PLAYER_DEAD;
				if (Listener)
					Listener->LogicPlayerDied(idx);
			}
			else
				p->TimeUntilNextState -= Delta;
		}

		if (p->State != PLAYER_DEAD)
			alivePlayerCount++;
	}

	if (alivePlayerCount <= 1)
		EndRound();
}

void TGameLogic::EndRound()
{
	// calculate player rank
	for (int idx = 0; idx < MAX_NUM_SLOTS; idx++)
	{
		TPlayer* p = &Data->Players[idx];
		if (p->State == PLAYER_NOTPLAYING)
			continue;

		if (p->State != PLAYER_DEAD)
			p->TimeOfDeath = Data->CurrentTime + 1; // after any other death

		p->Ranking = 1;

		for (int idx2 = 0; idx2 < idx; idx2++)
		{
			TPlayer* p2 = &Data->Players[idx2];
			if (p2->TimeOfDeath < p->TimeOfDeath) // p2 died earlier
				p2->Ranking++;
			else if (p2->TimeOfDeath > p->TimeOfDeath) // p1 died earlier
				p->Ranking++;
			else if (p2->TimeOfDeath == p->TimeOfDeath) // p1 and p2 died at the same time
				p2->Ranking = p->Ranking;
		}
	}

	// assign a point to the winner(s)
	for (int idx = 0; idx < MAX_NUM_SLOTS; idx++)
	{
		TPlayer* p = &Data->Players[idx];
		if (p->State == PLAYER_NOTPLAYING)
			continue;

		if (p->Ranking == 1)
			p->RoundsWon++;
	}

	// TODO: stop the round or the match
	if (Listener)
		Listener->LogicRoundEnded();
}

TFieldPosition TGameLogic::CalculatePlayerField(TPlayer* Player)
{
	TFieldPosition pos;
	pos.X = static_cast<int>(trunc(Player->Position.X));
	pos.Y = static_cast<int>(trunc(Player->Position.Y));
	return pos;
}

// TClientLogic

TClientLogic::TClientLogic(TGameData* SetData)
:	Data(SetData),
	GameLogic(SetData, this)
{
}

void TClientLogic::Process(TGameTime Delta)
{
	GameLogic.Process(Delta);
}

void TClientLogic::LogicBombExploding(const TFieldPosition& FieldPosition)
{
}

void TClientLogic::LogicBombExploded(const TFieldPosition& FieldPosition)
{
}

void TClientLogic::LogicPlayerDying(uint8_t Slot)
{
}

void TClientLogic::LogicPlayerDied(uint8_t Slot)
{
}

void TClientLogic::LogicRoundEnded()
{
}