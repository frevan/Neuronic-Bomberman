extends MultiplayerSynchronizer


signal OnDirectionChanged(NewDirection: Vector2)


@export var direction = Vector2()


func _process(_delta: float) -> void:
	if !get_parent().visible:
		return
	var old_direction = direction
	direction = Input.get_vector("player_left", "player_right", "player_up", "player_down")
	if old_direction != direction:
		OnDirectionChanged.emit(direction)
	pass
