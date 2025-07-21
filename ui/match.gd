extends TScene


signal OnLeaveLobby


func _process(_delta: float) -> void:
	if visible:
		_HandleUserInput()
		#UpdatePlayerFields()
		#if Variables.IsServer:
			#Rules.ExplodeBombs(context, delta)
			#Rules.RemoveExplosions(context, delta)
			#Rules.KillPlayersInExplosions(context)
			#Rules.CheckIfMatchEnded(context)
	pass


func _HandleUserInput() -> void:
	if Input.is_action_just_pressed("ui_cancel"):
		OnLeaveLobby.emit()
	#if Input.is_action_just_pressed("player_dropbomb"):
		#OnPlayerIsDroppingBomb.emit(true)
	#elif Input.is_action_just_released("player_dropbomb"):
		#OnPlayerIsDroppingBomb.emit(false)
	pass
