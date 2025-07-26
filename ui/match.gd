extends TScene


signal OnLeaveLobby


const bombscene = preload("res://items/bomb.tscn")
const explosionscene = preload("res://items/explosion.tscn")
const brickscene = preload("res://items/brick.tscn") 
const solidscene = preload("res://items/solidblock.tscn")

@onready var Rules: TRules = TRules.new()

var PlayerScenes: Dictionary # key = player_id
var Bombs: Dictionary # key: field (Vector2i) | value: scene (TBombScene)
var Explosions: Dictionary # key: field (Vector2i) | value: scene (TExplosionScene)


func BeforeShow() -> void:
	super()
	Client.OnNewRound.connect(_client_new_round)
	Client.OnPlayerPositionChanged.connect(_client_player_position_changed)
	Client.OnMapTileChanged.connect(_client_map_tile_changed)
	Client.OnBombDropped.connect(_client_bomb_dropped)
	Client.OnRemoveBomb.connect(_client_remove_bomb)
	Client.OnExplosion.connect(_client_explosion)
	Client.OnRemoveExplosion.connect(_client_remove_explosion)
	Client.OnPlayerDied.connect(_client_player_died)
	pass

func AfterHide() -> void:
	super()
	Client.OnNewRound.disconnect(_client_new_round)
	Client.OnPlayerPositionChanged.disconnect(_client_player_position_changed)
	Client.OnMapTileChanged.disconnect(_client_map_tile_changed)
	Client.OnBombDropped.disconnect(_client_bomb_dropped)
	Client.OnRemoveBomb.disconnect(_client_remove_bomb)
	Client.OnExplosion.disconnect(_client_explosion)
	Client.OnRemoveExplosion.disconnect(_client_remove_explosion)
	Client.OnPlayerDied.disconnect(_client_player_died)
	_CleanUpAfterMatch()
	pass


func _process(_delta: float) -> void:
	if visible:
		_HandleUserInput()
		_UpdatePlayerPositionsFromNodes()
	pass


func _HandleUserInput() -> void:
	if Input.is_action_just_pressed("ui_cancel"):
		OnLeaveLobby.emit()
	if Input.is_action_just_pressed("player_dropbomb"):
		Client.DropBombs(true)
	elif Input.is_action_just_released("player_dropbomb"):
		Client.DropBombs(false)
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

func _client_bomb_dropped(Field: Vector2i) -> void:
	var bomb = bombscene.instantiate()
	bomb.position = Tools.FieldPositionToScreen(Field)
	bomb.visible = true
	bomb.Field = Field
	Bombs[Field] = bomb
	add_child.call_deferred(bomb)
	pass

func _client_remove_bomb(Field: Vector2i) -> void:
	if Bombs.has(Field):
		var scene = Bombs[Field]
		Bombs.erase(Field)
		scene.queue_free()
	pass
	
func _client_explosion(Field: Vector2i) -> void:
	var e = explosionscene.instantiate()
	e.position = Tools.FieldPositionToScreen(Field)
	e.visible = true
	e.Field = Field
	Explosions[Field] = e
	add_child.call_deferred(e)
	pass
	
func _client_remove_explosion(Field: Vector2i) -> void:
	if Explosions.has(Field):
		var scene = Explosions[Field]
		Explosions.erase(Field)
		scene.queue_free()
	pass

func _client_player_died(PlayerID: int) -> void:
	if PlayerScenes.has(PlayerID):
		var scene = PlayerScenes[PlayerID]
		PlayerScenes.erase(PlayerID)
		scene.hide()
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
	_SetPlayerAuthorities()
	pass


func _FindPlayerNodeForSlot(SlotIndex: int) -> Node2D:
	var nodeName = "Player" + str(SlotIndex)
	return $Players.get_node(nodeName)


func _ShowPlayers() -> void:
	for i in Client.Data.Slots.size():
		var scene: Node2D = _FindPlayerNodeForSlot(i)
		assert(scene)
		var slot: Types.TSlot = Client.Data.Slots[i]
		scene.visible = (slot.PlayerID != 0) && slot.Alive
		PlayerScenes[slot.PlayerID] = scene
	pass


func _SetPlayerPositions() -> void:
	for i in Client.Data.Slots.size():
		var node = _FindPlayerNodeForSlot(i)
		assert(node)
		if node.visible:
			node.position = Tools.FieldPositionToScreen(Client.Data.Slots[i].Position)
	pass


func _SetPlayerAuthorities() -> void:
	for i in Client.Data.Slots.size():
		var node = _FindPlayerNodeForSlot(i)
		assert(node)
		if node.visible:
			var playerID: int = Client.Data.Slots[i].PlayerID
			node.set_multiplayer_authority(playerID)
			node.set_physics_process(playerID == Network.PeerID)
	pass


func _on_player_check_for_collisions(Sender: Node2D, NewPosition: Vector2) -> void:
	Sender.position = Rules.ApplyObstaclesToPlayerMove(Client.Data, Sender.position, NewPosition)
	pass


func _UpdatePlayerPositionsFromNodes() -> void:
	if visible:
		for i in Client.Data.Slots.size():
			var slot = Client.Data.Slots[i]
			if PlayerScenes.has(slot.PlayerID):
				var node: Node2D = PlayerScenes[slot.PlayerID]
				if !node:
					continue
				Client.UpdatePlayerPosition(slot.PlayerID, Tools.ScreenPositionToField(node.position))
			if slot.PlayerID == multiplayer.get_unique_id():
				if slot.Alive:
					var field = Vector2i(slot.Position)
					$PlayerPosLabel.text = str(field)
				else:
					$PlayerPosLabel.text = "dead"
	pass


func _RemovePlayerScenes() -> void:
	PlayerScenes.clear()
	pass

func _RemoveBombScenes() -> void:
	for field in Bombs:
		var scene = Bombs[field]
		scene.queue_free()
	Bombs.clear()
	pass

func _RemoveExplosionScenes() -> void:
	for field in Explosions:
		var scene = Explosions[field]
		scene.queue_free()
	Explosions.clear()
	pass

func _CleanUpAfterRound() -> void:
	_RemovePlayerScenes()
	_RemoveBombScenes()
	_RemoveExplosionScenes()
	pass


func _CleanUpAfterMatch() -> void:
	_CleanUpAfterRound()
	pass
