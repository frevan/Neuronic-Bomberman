extends Node

class_name TRules


func _LimitPlayerLeftToField(Position: Vector2, PlayerField: Vector2i) -> Vector2:
	var r: Rect2 = Tools.FieldToRect(PlayerField)
	return Vector2(r.position.x, Position.y)

func LimitPlayerRightToField(Position: Vector2, PlayerField: Vector2i) -> Vector2:
	var r: Rect2 = Tools.FieldToRect(PlayerField)
	return Vector2(r.position.x, Position.y)

func LimitPlayerTopToField(Position: Vector2, PlayerField: Vector2i) -> Vector2:
	var r: Rect2 = Tools.FieldToRect(PlayerField)
	return Vector2(Position.x, r.position.y)

func LimitPlayerBottomToField(Position: Vector2, PlayerField: Vector2i) -> Vector2:
	var r: Rect2 = Tools.FieldToRect(PlayerField)
	return Vector2(Position.x, r.position.y)

func _FieldHasBrickOrSolid(Map: Types.TMap, Field: Vector2i) -> bool:
	if (Field.x < Types.MAP_WIDTH) && (Field.y < Types.MAP_HEIGHT):
		return !Tools.FieldIsEmpty(Map.Fields[Field.y][Field.x])
	return false

func _FieldHasObstacle(Data: TGameData, Field: Vector2i) -> bool:
	return _FieldHasBrickOrSolid(Data.Map, Field) || Data.FieldHasBomb(Field)

func _PlayerTouchesField(Position: Vector2, Field: Vector2i) -> bool:
	var playerRect: Rect2 = Tools.PlayerPositionToRect(Position)
	var fieldRect: Rect2 = Tools.FieldToRect(Field)
	return playerRect.intersects(fieldRect)

func _LimitPlayerToArena(Position: Vector2) -> Vector2:
	Position.x = max(Position.x, Tools.FIELD_OFFSET.x)
	Position.x = min(Position.x, Tools.FIELD_OFFSET.x + (Tools.FIELD_SIZE.x * (Types.MAP_WIDTH - 1)))
	Position.y = max(Position.y, Tools.FIELD_OFFSET.y)
	Position.y = min(Position.y, Tools.FIELD_OFFSET.y + (Tools.FIELD_SIZE.y * (Types.MAP_HEIGHT - 1)))
	return Position

func _CheckIfPlayerCanMoveToField(Data: TGameData, NewPosition: Vector2, CurrentField: Vector2i, TestField: Vector2i) -> Vector2:
	if _FieldHasObstacle(Data, TestField):
		if _PlayerTouchesField(NewPosition, TestField):
			if TestField.x < CurrentField.x:
				NewPosition = _LimitPlayerLeftToField(NewPosition, CurrentField)
			elif TestField.x > CurrentField.x:
				NewPosition = LimitPlayerRightToField(NewPosition, CurrentField)
			elif TestField.y < CurrentField.y:
				NewPosition = LimitPlayerTopToField(NewPosition, CurrentField)
			elif TestField.y > CurrentField.y:
				NewPosition = LimitPlayerBottomToField(NewPosition, CurrentField)
	return NewPosition


func ApplyObstaclesToPlayerMove(Data: TGameData, Position: Vector2, NewPosition: Vector2) -> Vector2:
	var playerField: Vector2i = Tools.ScreenPositionToField(Position)
	NewPosition = _CheckIfPlayerCanMoveToField(Data, NewPosition, playerField, Vector2i(playerField.x - 1, playerField.y))
	NewPosition = _CheckIfPlayerCanMoveToField(Data, NewPosition, playerField, Vector2i(playerField.x + 1, playerField.y))
	NewPosition = _CheckIfPlayerCanMoveToField(Data, NewPosition, playerField, Vector2i(playerField.x, playerField.y - 1))
	NewPosition = _CheckIfPlayerCanMoveToField(Data, NewPosition, playerField, Vector2i(playerField.x, playerField.y + 1))
	return _LimitPlayerToArena(NewPosition)


func ApplyPowerupToPlayer(Data: TGameData, SlotIndex: int, Powerup: int) -> void:
	assert(Data)
	assert(SlotIndex != Constants.INVALID_SLOT)
	assert(SlotIndex < Data.Slots.size())
	var slot: TSlot = Data.Slots[SlotIndex]
	var p: TPowerup = Tools.CreatePowerup(Powerup)
	if p:
		p.ApplyToSlot(slot)
	pass

func OverridePlayerPowerup(Data: TGameData, SlotIndex: int, Powerup: int, Override: int) -> void:
	assert(Data)
	assert(SlotIndex != Constants.INVALID_SLOT)
	assert(SlotIndex < Data.Slots.size())
	var slot: TSlot = Data.Slots[SlotIndex]
	var p: TPowerup = Tools.CreatePowerup(Powerup)
	if p:
		p.ApplyOverrideToSlot(slot, Override)
	pass

func ApplyInitialPowerupsToPlayers(Data: TGameData) -> void:
	var _initial_powerups: Array
	for key in Data.Map.PowerUps:
		var pu: Types.TMapPowerUp = Data.Map.PowerUps[key]
		if pu.Number == Types.FIELD_PU_RANDOM - Types.FIELD_PU_FIRST:
			continue
		if pu.BornWith || pu.HasOverride:
			_initial_powerups.append(pu)
	
	for i in Data.Slots.size():
		for powerup: Types.TMapPowerUp in _initial_powerups:
			if powerup.BornWith:
				ApplyPowerupToPlayer(Data, i, powerup.Number)
			elif powerup.HasOverride:
				OverridePlayerPowerup(Data, i, powerup.Number, powerup.OverrideValue)
	pass
