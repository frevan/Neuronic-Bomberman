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
		return Map.Fields[Field.y][Field.x] != Types.FIELD_EMPTY
	return false

func _FieldHasObstacle(Map: Types.TMap, Field: Vector2i) -> bool:
	return _FieldHasBrickOrSolid(Map, Field) #|| _FieldHasBomb(Field)

func _PlayerTouchesField(Position: Vector2, Field: Vector2i) -> int:
	var playerRect: Rect2 = Tools.PlayerPositionToRect(Position)
	var fieldRect: Rect2 = Tools.FieldToRect(Field)
	return playerRect.intersects(fieldRect)

func _LimitPlayerToArena(Position: Vector2) -> Vector2:
	Position.x = max(Position.x, Tools.FIELD_OFFSET.x)
	Position.x = min(Position.x, Tools.FIELD_OFFSET.x + (Tools.FIELD_SIZE.x * (Types.MAP_WIDTH - 1)))
	Position.y = max(Position.y, Tools.FIELD_OFFSET.y)
	Position.y = min(Position.y, Tools.FIELD_OFFSET.y + (Tools.FIELD_SIZE.y * (Types.MAP_HEIGHT - 1)))
	return Position

func _CheckIfPlayerCanMoveToField(Map: Types.TMap, NewPosition: Vector2, CurrentField: Vector2i, TestField: Vector2i) -> Vector2:
	if _FieldHasObstacle(Map, TestField):
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


func ApplyObstaclesToPlayerMove(Map: Types.TMap, Position: Vector2, NewPosition: Vector2) -> Vector2:
	var playerField: Vector2i = Tools.ScreenPositionToField(Position)
	NewPosition = _CheckIfPlayerCanMoveToField(Map, NewPosition, playerField, Vector2i(playerField.x - 1, playerField.y))
	NewPosition = _CheckIfPlayerCanMoveToField(Map, NewPosition, playerField, Vector2i(playerField.x + 1, playerField.y))
	NewPosition = _CheckIfPlayerCanMoveToField(Map, NewPosition, playerField, Vector2i(playerField.x, playerField.y - 1))
	NewPosition = _CheckIfPlayerCanMoveToField(Map, NewPosition, playerField, Vector2i(playerField.x, playerField.y + 1))
	return _LimitPlayerToArena(NewPosition)
