extends TScene


signal OnLeaveLobby


const bombscene = preload("res://ui/items/bomb.tscn")
const explosionscene = preload("res://ui/items/explosion.tscn")
const brickscene = preload("res://ui/items/brick.tscn") 
const solidscene = preload("res://ui/items/solidblock.tscn")
const powerupscene = preload("res://ui/items/powerup.tscn")


const COLLISIONLAYER_BLOCK = 0b00000100_00000000
const COLLISIONLAYER_BOMB = 0b0001000_00000000

const COLLISIONMASK_ALL = 0xffffffff
const COLLISIONMASK_BLOCK = 0#0b00011111_11111111
const COLLISIONMASK_BOMB = 0b00011111_11111111


@onready var Rules: TRules = TRules.new()

var PlayerScenes: Dictionary # key = player_id
var Bombs: Dictionary # key: bomb_id (int) | value: scene (TBombScene)
var Explosions: Dictionary # key: field (Vector2i) | value: scene (TExplosionScene)

@onready var DoubleTapDetector: TDoubleTap = TDoubleTap.new()


func _log(Text: String) -> void:
	print(str(Network.PeerID) + " [match] " + Text)
	pass


func BeforeShow() -> void:
	super()
	Client.OnNewRound.connect(_client_new_round)
	Client.OnRoundStarted.connect(_client_round_started)
	Client.OnRoundEnded.connect(_client_round_ended)
	Client.OnPlayerPositionChanged.connect(_client_player_position_changed)
	Client.OnMapTileChanged.connect(_client_map_tile_changed)
	Client.OnBombDropped.connect(_client_bomb_dropped)
	Client.OnBombPosition.connect(_client_bomb_position)
	Client.OnBombStatusChanged.connect(_client_bomb_status_changed)
	Client.OnRemoveBomb.connect(_client_remove_bomb)
	Client.OnExplosion.connect(_client_explosion)
	Client.OnRemoveExplosion.connect(_client_remove_explosion)
	Client.OnPlayerDied.connect(_client_player_died)
	Client.OnCountDownStarted.connect(_client_countdown_started)
	Client.OnPlayerDisconnected.connect(_client_player_disconnected)
	pass

func AfterHide() -> void:
	super()
	Client.OnNewRound.disconnect(_client_new_round)
	Client.OnRoundStarted.disconnect(_client_round_started)
	Client.OnRoundEnded.disconnect(_client_round_ended)
	Client.OnPlayerPositionChanged.disconnect(_client_player_position_changed)
	Client.OnMapTileChanged.disconnect(_client_map_tile_changed)
	Client.OnBombDropped.disconnect(_client_bomb_dropped)
	Client.OnBombPosition.disconnect(_client_bomb_position)
	Client.OnBombStatusChanged.disconnect(_client_bomb_status_changed)
	Client.OnRemoveBomb.disconnect(_client_remove_bomb)
	Client.OnExplosion.disconnect(_client_explosion)
	Client.OnRemoveExplosion.disconnect(_client_remove_explosion)
	Client.OnPlayerDied.disconnect(_client_player_died)
	Client.OnCountDownStarted.disconnect(_client_countdown_started)
	Client.OnPlayerDisconnected.disconnect(_client_player_disconnected)
	_CleanUpAfterMatch()
	$CountDownTimer.stop()
	$RoundTimeTimer.stop()
	pass


func AfterShow() -> void:
	_CreateTiles()
	_ShowPlayers(false)
	pass


func _process(delta: float) -> void:
	if visible:
		_HandleUserInput(delta)
		_UpdatePlayerPositionsFromNodes()
		_MoveGrabbedBombNodes()
		_UpdateBombPositionsFromNodes()
		_RemoveCollisionExceptions()
	pass

func _HandleUserInput(delta: float) -> void:
	if Input.is_action_just_pressed("ui_cancel"):
		OnLeaveLobby.emit()
	if Client.State == Client.TState.ROUND && !Client.Data.CountingDown:
		_CheckPrimaryKey(delta)
		_CheckSecondaryKey(delta)
	pass

func _CheckPrimaryKey(delta: float) -> void:
	if Input.is_action_just_pressed("player_drop"):
		DoubleTapDetector.ProcessKeyDown()
		Client.KeyPressed(Constants.HOLDINGKEY_PRIMARY, true)
	if Input.is_action_just_released("player_drop"):
		Client.KeyPressed(Constants.HOLDINGKEY_PRIMARY, false)
		if DoubleTapDetector.ProcessKeyUp():
			Client.KeyPressed(Constants.DOUBLETAP_PRIMARY, true)
	DoubleTapDetector.Process(delta)
	pass

func _CheckSecondaryKey(_delta: float) -> void:
	if Input.is_action_just_pressed("player_kick"):
		Client.KeyPressed(Constants.HOLDINGKEY_SECONDARY, true)
	elif Input.is_action_just_released("player_kick"):
		Client.KeyPressed(Constants.HOLDINGKEY_SECONDARY, false)
	pass


func _client_new_round() -> void:
	#_log("new round")
	pass

func _client_round_started() -> void:
	#_log("round started")
	_ShowPlayers()
	_SetPlayerAuthorities()
	_SetPlayerPositions()
	_on_round_time_timer_timeout()
	if Client.Data.MaxTime > 0:
		$RoundTimeTimer.start()
	pass

func _client_round_ended() -> void:
	$RoundTimeTimer.stop()
	pass

func _client_player_position_changed(_PlayerID: int) -> void:
	_SetPlayerPositions()
	pass

func _client_map_tile_changed(_Field: Vector2i, _Type: int) -> void:
	_CreateTiles()
	pass

func _client_bomb_dropped(PlayerID: int, BombID: int, Type: int, Position: Vector2) -> void:
	var scene = bombscene.instantiate()
	scene.name = str(BombID)
	scene.PlayerID = PlayerID
	scene.PlayerSlotIndex = Client.Data.FindSlotForPlayer(PlayerID)
	scene.BombID = BombID
	scene.Type = Type
	scene.position = Tools.FieldPositionToScreen(Position) + Tools.BOMB_POS_OFFSET
	scene.visible = true
	scene.collision_layer = COLLISIONLAYER_BOMB
	scene.collision_mask = COLLISIONMASK_BOMB
	scene.Collided.connect(_bomb_collided)
	var fname: String = "res://assets/bomb.png"
	if Type == Constants.BOMB_TRIGGER: 
		fname = "res://assets/triggerbomb.png"
	scene.LoadSpriteFromFile(fname)
	scene.set_multiplayer_authority(1)
	Bombs[BombID] = scene
	$Bombs.add_child.call_deferred(scene)
	_AddCollisionExceptionsWithBomb(scene)
	pass

func _client_bomb_position(BombID: int, Position: Vector2) -> void:
	#_log("bomb position: " + str(Position))
	if Bombs.has(BombID):
		var scene = Bombs[BombID]
		scene.position = Tools.FieldPositionToScreen(Position) + Tools.BOMB_POS_OFFSET
	pass

func _client_bomb_status_changed(BombID: int) -> void:
	var bomb = Client.Data.GetBombForID(BombID)
	if bomb && Bombs.has(BombID):
		#_log("bomb status: punched=" + str(bomb.IsPunched))
		var scene = Bombs[BombID]
		scene.EnableCollisions(bomb.IsPunched)
	pass

func _client_remove_bomb(BombID: int) -> void:
	if Bombs.has(BombID):
		var scene = Bombs[BombID]
		Bombs.erase(BombID)
		scene.queue_free()
	pass
	
func _client_explosion(Field: Vector2i) -> void:
	var e = explosionscene.instantiate()
	e.position = Tools.FieldPositionToScreen(Field)
	e.visible = true
	e.Field = Field
	Explosions[Field] = e
	$ExplosionLayer.add_child.call_deferred(e)
	$ExplosionAudioPlayer.play()
	pass
	
func _client_remove_explosion(Field: Vector2i) -> void:
	if Explosions.has(Field):
		var scene = Explosions[Field]
		Explosions.erase(Field)
		scene.queue_free()
	pass

func _client_player_died(PlayerID: int) -> void:
	_RemovePlayerScene(PlayerID)
	pass

func _client_countdown_started(_CountDownTime: float) -> void:
	$CountDownTimer.start()
	_on_count_down_timer_timeout() # initial run to set the label text
	pass

func _client_player_disconnected(PlayerID: int) -> void:
	_RemovePlayerScene(PlayerID)
	pass


func _CreateTiles() -> void:
	for node in $Tiles.get_children():
		$Tiles.remove_child(node)
		node.queue_free()
		
	if Client.Data.Map:
		for row in Client.Data.Map.Fields.size():
			for col in Client.Data.Map.Fields[row].size():
				var field_type = Client.Data.Map.Fields[row][col]
				var field: Vector2i = Vector2i(col, row)
				_AddFieldNode(field, field_type)
				_AddPowerupNode(field, field_type)
	pass

func _AddFieldNode(Field: Vector2i, Type: int) -> void:
	var node: Node2D = null
	match Type:
		Types.FIELD_EMPTY: pass
		Types.FIELD_SOLID: node = solidscene.instantiate()
		Types.FIELD_BRICK: node = brickscene.instantiate()
	if node:
		node.collision_layer = COLLISIONLAYER_BLOCK
		node.collision_mask = COLLISIONMASK_BLOCK
		$Tiles.add_child(node, true)
		node.position = Tools.FieldPositionToScreen(Field)
	pass

func _AddPowerupNode(Field: Vector2i, Type: int) -> void:
	if !_FieldTypeIsPowerUp(Type):
		return
	var node: Node2D = powerupscene.instantiate()
	var fname: String = ""
	match Type:
		Types.FIELD_PU_EXTRABOMB: fname = "res://assets/powerup_extrabomb.png"
		Types.FIELD_PU_MOREFLAME: fname = "res://assets/powerup_moreflame.png"
		Types.FIELD_PU_DISEASE: fname = "res://assets/powerup_disease.png"
		Types.FIELD_PU_CANKICK: fname = "res://assets/powerup_kick.png"
		Types.FIELD_PU_EXTRASPEED: fname = "res://assets/powerup_speed.png"
		Types.FIELD_PU_CANPUNCH: fname = "res://assets/powerup_punch.png"
		Types.FIELD_PU_CANGRAB: fname = "res://assets/powerup_grab.png"
		Types.FIELD_PU_SPOOGER: fname = "res://assets/powerup_spooger.png"
		Types.FIELD_PU_GOLDFLAME: fname = "res://assets/powerup_maxflame.png"
		Types.FIELD_PU_TRIGGER: fname = "res://assets/powerup_trigger.png"
		Types.FIELD_PU_JELLYBOMBS: fname = "res://assets/powerup_jellybomb.png"
		Types.FIELD_PU_BADDISEASE: fname = "res://assets/powerup_superdisease.png"
		Types.FIELD_PU_RANDOM: fname = "res://assets/powerup_random.png"
	if fname != "":
		node.LoadSpriteFromFile(fname)
	$Tiles.add_child(node, true)
	node.position = Tools.FieldPositionToScreen(Field)
	pass

func _FieldTypeIsPowerUp(Type: int) -> bool:
	return (Type >= Types.FIELD_PU_FIRST) && (Type <= Types.FIELD_PU_LAST)


func _FindPlayerNodeForSlot(SlotIndex: int) -> Node2D:
	var nodeName = "Player" + str(SlotIndex)
	return $Players.get_node(nodeName)

func _FindPlayerNodeForID(PlayerID: int) -> Node2D:
	var slot: TSlot = Client.Data.GetSlotForPlayer(PlayerID)
	if slot:
		return _FindPlayerNodeForSlot(slot.Index)
	return null

func _GetPlayerSlotFromNode(Scene: Node2D) -> TSlot:
	var idx: int = Scene.SlotIndex
	if (idx >= 0) && (idx < Client.Data.Slots.size()):
		return Client.Data.Slots[idx]
	return null


func _ShowPlayers(CanShow: bool = true) -> void:
	#_log("show players - show=" + str(Show))
	for i in Client.Data.Slots.size():
		var scene: Node2D = _FindPlayerNodeForSlot(i)
		assert(scene)
		var slot: TSlot = Client.Data.Slots[i]
		scene.visible = CanShow && (slot.PlayerID != 0) && slot.Player.Alive
		scene.Data = Client.Data
		scene.SlotIndex = i
		scene.CreateSpriteShader()
		PlayerScenes[slot.PlayerID] = scene
	pass


func _SetPlayerPositions() -> void:
	for i in Client.Data.Slots.size():
		var node = _FindPlayerNodeForSlot(i)
		assert(node)
		if node.visible:
			node.position = Tools.FieldPositionToScreen(Client.Data.Slots[i].Player.Position)
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


func _on_player_check_for_collisions(_Sender: Node2D, _NewPosition: Vector2) -> void:
	#if Client.State == Client.TState.ROUND:
		#Sender.position = Rules.ApplyObstaclesToPlayerMove(Client.Data, Sender.position, NewPosition)
	pass


func _UpdatePlayerPositionsFromNodes() -> void:
	if visible && (Client.State == Client.TState.ROUND):
		for i in Client.Data.Slots.size():
			var slot = Client.Data.Slots[i]
			if PlayerScenes.has(slot.PlayerID):
				var node: Node2D = PlayerScenes[slot.PlayerID]
				if !node:
					continue
				Client.UpdatePlayerPosition(slot.PlayerID, Tools.ScreenPositionToField(node.position))
			#if slot.PlayerID == multiplayer.get_unique_id():
				#if slot.Player.Alive:
					#var field = Vector2i(slot.Player.Position)
					#$PlayerPosLabel.text = str(field)
				#else:
					#$PlayerPosLabel.text = "dead"
	pass

func _UpdateBombPositionsFromNodes() -> void:
	if visible && (Client.State == Client.TState.ROUND):
		for i in $Bombs.get_child_count():
			var scene = $Bombs.get_child(i)
			if scene:
				Client.UpdateBombPosition(scene.BombID, Tools.ScreenPositionToField(scene.position))
	pass

func _MoveGrabbedBombNodes() -> void:
	for b in Bombs.values():
		var bomb: TBomb = Client.Data.GetBombForID(b.BombID)
		if bomb:
			if bomb.IsGrabbedBy != Constants.INVALID_SLOT:
				var playerscene: Node2D = PlayerScenes[bomb.IsGrabbedBy]
				if playerscene:
					b.position = playerscene.position + Vector2(0, -Tools.FIELD_SIZE.y)
	pass


func _AddCollisionExceptionsWithBomb(Scene: Node2D) -> void:
	for i in Client.Data.Slots.size():
		var slot: TSlot = Client.Data.Slots[i]
		var pscene = _FindPlayerNodeForID(slot.PlayerID)
		if !pscene:
			return
		var playerrect = Tools.PositionToRect(pscene.position)
		var bombrect = Tools.PositionToRect(Scene.position)
		if bombrect.intersects(playerrect):
			Scene.add_collision_exception_with(pscene)
	pass


func _RemoveCollisionExceptions() -> void:
	if visible && (Client.State == Client.TState.ROUND):
		for i in Client.Data.Slots.size():
			var slot: TSlot = Client.Data.Slots[i]
			var pscene = _FindPlayerNodeForID(slot.PlayerID)
			if !pscene:
				return
			var playerrect = Tools.PositionToRect(pscene.position)
			for id in Bombs:
				var bscene: Node2D = Bombs[id]
				var bombrect = Tools.PositionToRect(bscene.position)
				if !bombrect.intersects(playerrect):
					bscene.remove_collision_exception_with(pscene)
	pass

func _RemovePlayerScenes() -> void:
	PlayerScenes.clear()
	pass

func _ResetPlayerSceneDirections() -> void:
	for i in Client.Data.Slots.size():
		var node = _FindPlayerNodeForSlot(i)
		if node:
			node.ResetSpriteDirection()
	pass


func _RemoveBombScenes() -> void:
	for id in Bombs:
		var scene = Bombs[id]
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
	_ResetPlayerSceneDirections()
	_RemovePlayerScenes()
	_RemoveBombScenes()
	_RemoveExplosionScenes()
	pass


func _CleanUpAfterMatch() -> void:
	_CleanUpAfterRound()
	pass


func _on_count_down_timer_timeout() -> void:
	if !Client.Data.CountingDown:
		$CountDownTimer.stop()
	_UpdateCountDownLabelText()
	pass


func _UpdateCountDownLabelText() -> void:
	var s: String
	match Client.Data.WinCondition:
		Constants.WinCondition.NUM_ROUNDS: s = "Round " + str(Client.Data.CurrentRound + 1) + " of " + str(Client.Data.NumRounds)
		Constants.WinCondition.SCORE: s = "First to " + str(Client.Data.ScoreToWin) + " wins"
	if Client.Data.CountingDown:
		if s != "":
			s += " - "
		var _time: int = floor(Client.Data.CountDownTime)
		if _time <= 0:
			s += "GO!"
		else:
			s += str(_time)
	$CountDownLabel.text = s
	pass


func _RemovePlayerScene(PlayerID: int) -> void:
	if PlayerScenes.has(PlayerID):
		var scene = PlayerScenes[PlayerID]
		PlayerScenes.erase(PlayerID)
		scene.hide()
	pass


func _on_player_direction_changed(_Sender: Node2D, NewDirection: Vector2) -> void:
	var d = _VectorToDirection(NewDirection)
	if d != Constants.DIRECTION_NONE:
		Client.SetPlayerDirection(d)
	pass

func _VectorToDirection(Vector: Vector2) -> int:
	if absf(Vector.y) > absf(Vector.x):
		if Vector.y > 0:
			return Constants.DIRECTION_DOWN
		elif Vector.y < 0:
			return Constants.DIRECTION_UP
	else:
		if Vector.x > 0:
			return Constants.DIRECTION_RIGHT
		elif Vector.x < 0:
			return Constants.DIRECTION_LEFT
	return Constants.DIRECTION_NONE


func _on_player_collided_with_bomb(Sender: TPlayerScene, Bomb: TBombScene, _Collision: KinematicCollision2D) -> void:
	var player_input: Object = Sender.get_node("PlayerInput")
	if player_input:
		if player_input.direction != Vector2.ZERO:
			var slot: TSlot = Client.Data.Slots[Sender.SlotIndex]
			if slot.Player.CanKick:
				Bomb.direction = player_input.direction
				Client.SendBombIsMoving(Bomb.BombID, true)
	pass

func _bomb_collided(Sender: TBombScene) -> void:
	var bomb: TBomb = Client.Data.GetBombForID(Sender.BombID)
	if bomb:
		if bomb.IsJelly:
			Sender.direction = Sender.direction * -1.0
		else:
			Sender.direction = Vector2.ZERO
			var field = Tools.ScreenPositionToField(Sender.position)
			Sender.position = Tools.FieldPositionToScreen(field) + Tools.BOMB_POS_OFFSET
			Client.SendBombIsMoving(Sender.BombID, false)
	pass


func _on_round_time_timer_timeout() -> void:
	var s: String = ""
	if Client.Data.MaxTime > 0:
		var seconds: int = maxi(0, int(fmod(Client.Data.CurrentRoundTime, 60.0)))
		var minutes: int = maxi(0, int(Client.Data.CurrentRoundTime / 60.0))
		s = "%02d:%02d" % [minutes, seconds]
	$RoundTimeLabel.text = s
	pass
