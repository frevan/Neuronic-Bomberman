extends MultiplayerSynchronizer


signal DirectionChanged


@export var direction = Vector2()


func _process(delta: float) -> void:
	if !get_parent().visible:
		return
	direction = Input.get_vector("player_left", "player_right", "player_up", "player_down")
	DirectionChanged.emit(delta)
	pass
