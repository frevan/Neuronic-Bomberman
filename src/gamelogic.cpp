#include "gamelogic.h"

const float SmallestDistanceConst = 0.000001f;
const float CornerAnimationIncConst = 0.05f;

TGameLogic::TGameLogic(TGame* SetGame)
:	Game(SetGame)
{
}

TGameLogic::~TGameLogic()
{
}

void TGameLogic::Process(TGameTime Delta)
{
	if (Game->CurrentState != STATE_MATCH)
		return;

	UpdatePlayerPositions(Delta);
	UpdateBombs(Delta);
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
							v = fabs(diff) < CornerAnimationIncConst ? fabs(diff) : CornerAnimationIncConst;
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
							v = fabs(diff) < CornerAnimationIncConst ? fabs(diff) : CornerAnimationIncConst;
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
	float curpos, nextpos;
	size_t mapsize;
	otherDirection = DIRECTION_NONE;

	switch (Direction)
	{
		case DIRECTION_LEFT:
			curpos = Player->Position.X;
			nextpos = Player->Position.X - Distance;
			nextfieldx = (int)nextpos;
			nextfieldy = (int)Player->Position.Y;
			mapsize = Game->GameData.Arena.Width;
			break;
		case DIRECTION_RIGHT:
			curpos = Player->Position.X;
			nextpos = Player->Position.X + Distance;
			nextfieldx = (int)nextpos;
			nextfieldy = (int)Player->Position.Y;
			mapsize = Game->GameData.Arena.Width;
			break;
		case DIRECTION_UP:
			curpos = Player->Position.Y;
			nextpos = Player->Position.Y - Distance;
			nextfieldx = (int)Player->Position.X;
			nextfieldy = (int)nextpos;
			mapsize = Game->GameData.Arena.Height;
			break;
		case DIRECTION_DOWN:
			curpos = Player->Position.Y;
			nextpos = Player->Position.Y + Distance;
			nextfieldx = (int)Player->Position.X;
			nextfieldy = (int)nextpos;
			mapsize = Game->GameData.Arena.Height;
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
		float dtr = fabs(curfield + 0.5f - curpos);
		switch (Direction)
		{
			case DIRECTION_LEFT:
			case DIRECTION_UP:
				if (fabs(curpos - curfield) < 0.5f)
					dtr = 0;
				break;
			case DIRECTION_RIGHT:
			case DIRECTION_DOWN:
				if (fabs(curpos - curfield) > 0.5f)
					dtr = 0;
				break;
		}

		return dtr;
	}
	else
		return fabs(nextpos - curpos);
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
	if (CellX < 0 || CellX >= Game->GameData.Arena.Width || CellY < 0 || CellY >= Game->GameData.Arena.Height)
		return false;

	// check cell contents to see if we can move there
	TField* cell = Game->GameData.Arena.At(CellX, CellY);
	bool canmove = true;
	canmove &= (cell->Type != FIELD_BRICK && cell->Type != FIELD_SOLID);
	canmove &= !Game->GameData.BombInField(CellX, CellY);
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
		if (x >= 0 && x < Game->GameData.Arena.Width && y >= 0 && y < Game->GameData.Arena.Height)
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
		TPlayer* player = &Game->GameData.Players[i];

		if (player->Direction == 0)
			player->Speed = SPEED_NOTMOVING;
		else
			player->Speed = SPEED_NORMAL;

		float normalizedSpeed = player->Speed / (float)SPEED_NORMAL;
		float distanceTravelled = normalizedSpeed * (float)Delta / 1000.f * 1.5f;

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
	for (uint8_t x = 0; x < Game->GameData.Arena.Width; x++)
		for (uint8_t y = 0; y < Game->GameData.Arena.Height; y++)
		{
			TField* field = Game->GameData.Arena.At(x, y);

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
					field->Bomb.State = BOMB_NONE;
				else
					field->Bomb.TimeUntilNextState -= Delta;
			}
		}
}

void TGameLogic::ExplodeBomb(uint8_t X, uint8_t Y)
{
	TField* field;

	// explode the field itself
	field = Game->GameData.Arena.At(X, Y);
	field->Bomb.State = BOMB_EXPLODING;
	field->Bomb.TimeUntilNextState = 1000; // explode for 1 second

	// explode the fields to the left of the current field
	for (int i = 1; i <= field->Bomb.Range; i++)
	{
		if (X - i < 0)
			break;
		TField* field2 = Game->GameData.Arena.At(X - i, Y);
		if (field2->Type == FIELD_SOLID)
			break;
		if (field2->Type == FIELD_BRICK)
			field2->Type = FIELD_EMPTY;

		field2->Bomb = field->Bomb;
	}
	// explode the fields to the right of the current field
	for (int i = 1; i <= field->Bomb.Range; i++)
	{
		if (X + i >= Game->GameData.Arena.Width)
			break;
		TField* field2 = Game->GameData.Arena.At(X + i, Y);
		if (field2->Type == FIELD_SOLID)
			break;
		if (field2->Type == FIELD_BRICK)
			field2->Type = FIELD_EMPTY;

		field2->Bomb = field->Bomb;
	}
	// explode the fields above the current field
	for (int i = 1; i <= field->Bomb.Range; i++)
	{
		if (Y - i < 0)
			break;
		TField* field2 = Game->GameData.Arena.At(X, Y - i);
		if (field2->Type == FIELD_SOLID)
			break;
		if (field2->Type == FIELD_BRICK)
			field2->Type = FIELD_EMPTY;

		field2->Bomb = field->Bomb;
	}
	// explode the fields below the current field
	for (int i = 1; i <= field->Bomb.Range; i++)
	{
		if (Y + i >= Game->GameData.Arena.Height)
			break;
		TField* field2 = Game->GameData.Arena.At(X, Y + i);
		if (field2->Type == FIELD_SOLID)
			break;
		if (field2->Type == FIELD_BRICK)
			field2->Type = FIELD_EMPTY;

		field2->Bomb = field->Bomb;
	}
}