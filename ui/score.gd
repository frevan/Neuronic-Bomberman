extends TScene

signal OnReadyForNextRound
signal OnLeaveMatch


func _log(Text: String) -> void:
	print(str(Network.PeerID) + " [score] " + Text)
	pass


func _process(_delta: float) -> void:
	if visible:
		_HandleUserInput()
	pass


func _HandleUserInput() -> void:
	if Input.is_action_just_pressed("ui_cancel"):
		if Client.State == Client.TState.LOBBY:
			_ContinueToMatchOrLobby()
	if Input.is_action_just_pressed("ui_accept"):
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
		_UpdateScoresLabel()
	pass


func _DetermineHighestScore() -> int:
	var highest_score: int = 0
	for slot: Types.TSlot in Client.Data.Slots:
		highest_score = max(slot.Score, highest_score)
	return highest_score


func _DetermineMyScore() -> int:
	for slot: Types.TSlot in Client.Data.Slots:
		if slot.PlayerID == Network.PeerID:
			return slot.Score
	return -1


func _IsThereATieForTheWin(HighestScore: int) -> bool:
	var count: int = 0
	for slot: Types.TSlot in Client.Data.Slots:
		if slot.Score == HighestScore:
			count += 1
	return count > 1


func _UpdateMessageLabel():
	var highest_score: int = _DetermineHighestScore()
	var my_score: int = _DetermineMyScore()
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


func _UpdateScoresLabel() -> void:
	_log("scores = " + str(Client.Data.Slots[0].Score) + " " + str(Client.Data.Slots[1].Score))
	var s: String = ""
	var highest_score: int = _DetermineHighestScore()
	for score in range(highest_score, -1, -1):
		_log("  score=" + str(score))
		for idx in Client.Data.Slots.size():
			var slot: Types.TSlot = Client.Data.Slots[idx]
			if slot.PlayerID == 0:
				continue
			_log("    slot=" + str(slot.PlayerID) + ": " + str(slot.Score))
			if slot.Score == score:
				s += "\n" + str(slot.PlayerID) + " - " + str(slot.Score)
				_log("  s=" + s)
	$ScoresLabel.text = s
	pass


func _on_continue_btn_pressed() -> void:
	_ContinueToMatchOrLobby()
	pass
