extends TScene


signal OnLeaveLobby


const brickscene = preload("res://items/brick.tscn") 
const solidscene = preload("res://items/solidblock.tscn")

@onready var Rules: TRules = TRules.new()


func BeforeShow() -> void:
	super()
	Client.OnNewRound.connect(_client_new_round)
	Client.OnPlayerPositionChanged.connect(_client_player_position_changed)
	Client.OnMapTileChanged.connect(_client_map_tile_changed)
	pass

func AfterHide() -> void:
	super()
	Client.OnNewRound.disconnect(_client_new_round)
	Client.OnPlayerPositionChanged.disconnect(_client_player_position_changed)
	Client.OnMapTileChanged.disconnect(_client_map_tile_changed)
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

func _client_player_position_changed(_PlayerID: int) -> void:
	_SetPlayerPositions()
	pass

func _client_map_tile_changed(_Field: Vector2i, _Type: int) -> void:
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
					node.position = Tools.FieldPositionToScreen(Vector2(col, row))
	pass


func UpdateAll() -> void:
	_CreateTiles()
	_ShowPlayers()
	_SetPlayerPositions()
	pass


func _FindPlayerNodeForSlot(SlotIndex: int) -> Node2D:
	var nodeName = "Player" + str(SlotIndex)
	return $Players.get_node(nodeName)


func _ShowPlayers() -> void:
	for i in Client.Data.Slots.size():
		var scene: Node2D = _FindPlayerNodeForSlot(i)
		assert(scene)
		var p: Types.TPlayer = Client.Data.Slots[i].Player
		scene.visible = p.PeerID != 0
	pass


func _SetPlayerPositions() -> void:
	for i in Client.Data.Slots.size():
		var node = _FindPlayerNodeForSlot(i)
		assert(node)
		if node.visible:
			var p: Types.TPlayer = Client.Data.Slots[i].Player
			node.position = Tools.FieldPositionToScreen(p.Position)
	pass


func _on_player_check_for_collisions(Sender: Node2D, NewPosition: Vector2) -> void:
	Sender.position = Rules.ApplyObstaclesToPlayerMove(Client.Data.Map, Sender.position, NewPosition)
	pass
