extends TScene

signal OnReadyForNextRound
signal OnLeaveMatch


func _process(_delta: float) -> void:
	if visible:
		_HandleUserInput()
	pass


func _HandleUserInput() -> void:
	if Input.is_action_just_pressed("ui_cancel") || Input.is_action_just_pressed("ui_accept"):
		_ContinueToMatchOrLobby()
	pass

func _input(event) -> void:
	if !visible:
		return
	if event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_LEFT:
			_ContinueToMatchOrLobby()
	pass

func _ContinueToMatchOrLobby() -> void:
	if Client.State == Client.TState.LOBBY:
		OnLeaveMatch.emit()
	elif Client.State == Client.TState.MATCH:
		OnReadyForNextRound.emit()
