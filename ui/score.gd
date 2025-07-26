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

#func _input(event) -> void:
	#if !visible:
		#return
	#if event is InputEventMouseButton:
		#if event.button_index == MOUSE_BUTTON_LEFT:
			#_ContinueToMatchOrLobby()
	#pass

func _ContinueToMatchOrLobby() -> void:
	if Client.State == Client.TState.LOBBY:
		OnLeaveMatch.emit()
	elif Client.State == Client.TState.MATCH:
		OnReadyForNextRound.emit()
	pass


func _on_visibility_changed() -> void:
	if visible:
		_UpdateMessageLabel()
	pass


func _IsThereATieForTheWin(HighestScore: int) -> bool:
	var count: int = 0
	for slot: Types.TSlot in Client.Data.Slots:
		if slot.Score == HighestScore:
			count += 1
	return count > 1


func _UpdateMessageLabel():
	var highest_score: int = 0
	var my_score: int = 0
	for slot: Types.TSlot in Client.Data.Slots:
		highest_score = max(slot.Score, highest_score)
		if slot.PlayerID == Network.PeerID:
			my_score = slot.Score
	var tied: bool = _IsThereATieForTheWin(highest_score)
	
	var msg: String = "We're done here"	
	match Client.State:
		Client.TState.LOBBY:
			var idx = Client.Data.FindSlotForPlayer(Network.PeerID)
			if idx != Types.INVALID_SLOT:
				if my_score == highest_score:
					if tied:
						msg = "You tied for the win!"
					else:
						msg = "You won!"
				else:
					msg = "You lost!"
		Client.TState.MATCH:
			msg = "On to the next round..."
	$MessageLabel.text = msg
	pass
