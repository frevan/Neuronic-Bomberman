extends TScene


signal OnLeaveLobby


const brickscene = preload("res://items/brick.tscn") 
const solidscene = preload("res://items/solidblock.tscn")


func BeforeShow() -> void:
	super()
	Client.OnNewRound.connect(_client_new_round)
	pass

func AfterHide() -> void:
	super()
	Client.OnNewRound.disconnect(_client_new_round)
	pass


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


func _client_new_round() -> void:
	_CreateTiles()
	pass


func _CreateTiles() -> void:
	for node in $Tiles.get_children():
		$Tiles.remove_child(node)
		node.queue_free()
		
	if Client.Data.Map:
		for row in Client.Data.Map.Fields.size():
			for col in Client.Data.Map.Fields[row].size():
				var node = null
				match Client.Data.Map.Fields[row][col]:
					Types.FIELD_EMPTY: pass
					Types.FIELD_SOLID: node = solidscene.instantiate()
					Types.FIELD_BRICK: node = brickscene.instantiate()
				if node:
					$Tiles.add_child(node, true)
					node.position = Tools.FieldToPosition(Vector2i(col, row))
	pass


func UpdateAll() -> void:
	_CreateTiles()
	pass
