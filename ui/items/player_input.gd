extends MultiplayerSynchronizer


signal OnDirectionChanged(NewDirection: Vector2)


@export var direction = Vector2()


func _process(_delta: float) -> void:
	if !get_parent().visible:
		return
	var old_direction: Vector2 = direction
	var new_direction: Vector2 = Input.get_vector("player_left", "player_right", "player_up", "player_down")
	direction = _LimitDirection(new_direction)
	if old_direction != direction:
		OnDirectionChanged.emit(direction)
	pass


func _LimitDirection(Value: Vector2) -> Vector2:
	if Value != Vector2.ZERO:
		if absf(Value.x) < 0.5:
			Value.x = 0.0
		if absf(Value.y) < 0.5:
			Value.y = 0.0
		if absf(Value.x) >= absf(Value.y):
			return Vector2(1.0 * signf(Value.x), 0.0)
		else:
			return Vector2(0.0, 1.0 * signf(Value.y))
	return Value
