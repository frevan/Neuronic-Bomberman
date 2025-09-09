extends Node

class_name TServer


const COUNTDOWN_TIME = 5

const NUM_ROUNDS_MIN = 1
const NUM_ROUNDS_MAX = 100

var Data: TGameData
var Maps: TMaps
var CurrentMapName: String
var Comms: TServerComms

enum TState {IDLE, LOBBY, MATCH, ROUND}
var State: TState = TState.IDLE


@onready var Rules: TRules = TRules.new()


func _log(Text: String) -> void:
	print(str(Network.PeerID) + " [srv] " + Text)
	pass


func _ready() -> void:
	set_process(false)
	pass


func _process(delta: float) -> void:
	if !Data:
		return
	if State == TState.MATCH:
		_ProcessMatch(delta)
	elif State == TState.ROUND:
		_ProcessPlayers(delta)
		_ProcessBombs(delta)
		_RemoveExplosions(delta)
		_KillPlayersInExplosions()
		_PickUpPowerups()
		_ProcessDiseases(delta)
	pass


func _ProcessMatch(delta: float) -> void:
	if Data.CountingDown:
		Data.CountDownTime -=  delta
		if Data.CountDownTime <= 0:
			Data.CountingDown = false
			_StartRoundNow()
	pass


func _peer_connected(SenderID: int) -> void:
	_log("peer connected: " + str(SenderID))
	pass


func _peer_disconnected(SenderID: int) -> void:
	_log("peer disconnected: " + str(SenderID))
	_ClientDisconnected(SenderID)
	pass


func _ProcessPlayers(Delta: float) -> void:
	for i in Data.Slots.size():
		var slot: TSlot = Data.Slots[i]
		if (slot.PlayerID == 0) or !slot.Player.Alive:
			continue
		slot.Player.TimeBeforeNextTriggeredBomb -= Delta
		if slot.Player.HoldingPrimaryKey || slot.Player.Diseases[Constants.DISEASE_DIARRHEA]:
			var r: bool = false
			if slot.Player.HasGrabbedBombID == Constants.INVALID_BOMB_ID:
				r = _DropBomb(slot, slot.Player.Position)
			if r:
				slot.Player.CanGrabBombNow = false
			elif slot.Player.PrimaryAction == Constants.PA_GRABBOMB && slot.Player.CanGrabBombNow:
				_GrabBomb(slot)
		elif !slot.Player.HoldingPrimaryKey && (slot.Player.HasGrabbedBombID != Constants.INVALID_BOMB_ID):
				var bombid: int = Data.FindBombGrabbedBy(slot.PlayerID)
				if bombid != Constants.INVALID_BOMB_ID:
					_ThrowBomb(bombid)
		if slot.Player.HoldingSecondaryKey:
			var hasPunched: bool = false
			if slot.Player.CanPunch:
				hasPunched = _PunchBombNextToPlayer(slot)
			if !hasPunched:
				_ExplodeTriggerBombs(slot)
	pass

func _DropBomb(Slot: TSlot, Field: Vector2i) -> bool:
	var will_drop = false
	if Slot.Player.Diseases[Constants.DISEASE_DIARRHEA] > 0:
		will_drop = true
	else:
		var total = Slot.Player.TotalBombs
		var dropped = Slot.Player.DroppedBombs
		var constipated = Slot.Player.Diseases[Constants.DISEASE_CONSTIPATION] > 0
		will_drop = (dropped < total) && !constipated
		
	if will_drop:
		var id: int = Tools.NewBombID()
		var type = Constants.BOMB_NORMAL
		if Slot.Player.NumTriggerBombs > 0:
			type = Constants.BOMB_TRIGGER
		if Data.AddBombAt(Slot.PlayerID, id, type, Field):
			Slot.Player.DroppedBombs += 1
			Network.SendBombDropped.rpc(Slot.PlayerID, id, type, Field)
			return true
	return false

func _ExplodeTriggerBombs(Slot: TSlot) -> void:
	var bomb: TBomb = Data.FindOldestTriggerBombFor(Slot.PlayerID)
	if bomb && (Slot.Player.TimeBeforeNextTriggeredBomb <= 0):
		_ExplodeBomb(bomb)
	pass

func _PunchBombNextToPlayer(Slot: TSlot) -> bool:
	var direction: Vector2i = Vector2i.ZERO	
	match Slot.Player.Direction:
		Constants.DIRECTION_LEFT: direction = Vector2i(-1, 0)
		Constants.DIRECTION_RIGHT: direction = Vector2i(1, 0)
		Constants.DIRECTION_UP: direction = Vector2i(0, -1)
		Constants.DIRECTION_DOWN: direction = Vector2i(0, 1)
	var field: Vector2i = Vector2i(Slot.Player.Position) + direction
	if (field >= Vector2i.ZERO) && (field < Vector2i(Types.MAP_WIDTH, Types.MAP_HEIGHT)):
		var bomb: TBomb = Data.GetBombInField(field)
		if bomb:
			if !bomb.IsMoving:
				_PunchBomb(bomb, direction)
				return true
	return false

func _PunchBomb(Bomb: TBomb, Direction: Vector2i) -> void:
	Bomb.IsMoving = true
	Bomb.IsPunched = true
	Bomb.PunchDirection = Direction
	Bomb.PunchStartField = Vector2i(Bomb.Position)
	Bomb.PunchEndField = Bomb.PunchStartField + (Direction * 4)
	Network.SendBombStatus.rpc(Bomb.ID, Bomb.ToJSONString())
	pass

func _GrabBomb(Slot: TSlot) -> void:
	var bomb: TBomb = Data.GetBombInField(Slot.Player.Position)
	if !bomb:
		return
	bomb.IsMoving = true
	bomb.IsGrabbedBy = Slot.PlayerID
	Network.SendBombStatus.rpc(bomb.ID, bomb.ToJSONString())
	Slot.Player.HasGrabbedBombID = bomb.ID
	pass

func _ThrowBomb(BombID: int) -> void:
	var bomb: TBomb = Data.GetBombForID(BombID)
	if !bomb:
		return
	var slot: TSlot = Data.GetSlotForPlayer(bomb.IsGrabbedBy)
	bomb.IsMoving = false
	bomb.IsGrabbedBy = Constants.INVALID_BOMB_ID
	Network.SendBombStatus.rpc(bomb.ID, bomb.ToJSONString())
	if slot:
		slot.Player.HasGrabbedBombID = Constants.INVALID_BOMB_ID
	pass


func _ProcessBombs(Delta: float) -> void:
	for id in Data.Bombs:
		var bomb: TBomb = Data.Bombs[id]
		bomb.TimeSinceDrop += Delta
		if !bomb.IsMoving:
			bomb.TimeUntilExplosion -= Delta
			if (bomb.Type == Constants.BOMB_NORMAL) && (bomb.TimeUntilExplosion <= 0):
				_ExplodeBomb(bomb)
		elif bomb.IsPunched:
			_MovePunchedBomb(Delta, bomb)
		elif bomb.IsGrabbedBy != Constants.INVALID_SLOT:
			_MoveGrabbedBomb(bomb)
	pass
	
func _CalculateNewBombPosition(Delta: float, Bomb: TBomb) -> Vector2:
	var speed: float = Delta * Constants.BOMB_SPEED_KICK
	var r = Bomb.Position + (Vector2(Bomb.PunchDirection) * speed)
	var total_distance: float
	var cur_distance: float
	if Bomb.PunchDirection.x != 0:
		total_distance = Bomb.PunchStartField.x - Bomb.PunchEndField.x
		cur_distance = Bomb.Position.x - Bomb.PunchStartField.x
		r.y = Bomb.PunchStartField.y + sin(cur_distance / total_distance * PI)
	elif Bomb.PunchDirection.y != 0:
		total_distance = absf(Bomb.PunchStartField.y - Bomb.PunchEndField.y)
		cur_distance = absf(Bomb.Position.y - Bomb.PunchStartField.y)
		r.x = Bomb.PunchStartField.x + sin(cur_distance / total_distance * PI)
	return r

func _MovePunchedBomb(Delta: float, Bomb: TBomb) -> void:
	Bomb.Position = _CalculateNewBombPosition(Delta, Bomb)
	var field: Vector2i = Vector2i(Bomb.Position)
	if abs(field - Bomb.PunchStartField) >= abs(Bomb.PunchEndField - Bomb.PunchStartField):
		if Maps.GetFieldType(Data.Map, field) == Types.FIELD_EMPTY:
			Bomb.IsMoving = false
			Bomb.IsPunched = false
			Bomb.Position = Bomb.PunchEndField
			Network.SendBombStatus(Bomb.ID, Bomb.ToJSONString())
		else:
			Bomb.PunchEndField += Bomb.PunchDirection
	else:
		if Bomb.PunchDirection.x < 0 && Bomb.PunchEndField.x < 0 && field.x < 0:
			Bomb.PunchEndField.x = Types.MAP_WIDTH - 1
			Bomb.Position.x = Types.MAP_WIDTH
		elif Bomb.PunchDirection.x > 0 && Bomb.PunchEndField.x >= Types.MAP_WIDTH && field.x > Types.MAP_WIDTH:
			Bomb.PunchEndField.x = 0
			Bomb.Position.x = -1
		elif Bomb.PunchDirection.y < 0 && Bomb.PunchEndField.y < 0 && field.y < 0:
			Bomb.PunchEndField.y = Types.MAP_HEIGHT - 1
			Bomb.Position.y = Types.MAP_HEIGHT
		elif Bomb.PunchDirection.y > 0 && Bomb.PunchEndField.y >= Types.MAP_HEIGHT && field.y > Types.MAP_HEIGHT:
			Bomb.PunchEndField.y = 0
			Bomb.Position.y = -1
	Network.SendBombPosition.rpc(Bomb.ID, Bomb.Position)
	pass

func _MoveGrabbedBomb(Bomb: TBomb) -> void:
	if Bomb.IsGrabbedBy == Constants.INVALID_SLOT:
		return
	var slot: TSlot = Data.GetSlotForPlayer(Bomb.IsGrabbedBy)
	if !slot:
		return
	Bomb.Position = slot.Player.Position
	Network.SendBombPosition.rpc(Bomb.ID, Bomb.Position)
	pass


func _ExplodeBomb(Bomb: TBomb) -> void:
	_RemoveBomb(Bomb)
	_CreateExplosionsForBomb(Bomb)
	if Bomb.Type == Constants.BOMB_TRIGGER:
		var idx = Data.FindSlotForPlayer(Bomb.PlayerID)
		if idx != Constants.INVALID_SLOT:
			var slot: TSlot = Data.Slots[idx]
			slot.Player.TimeBeforeNextTriggeredBomb = Constants.TIME_BETWEEN_TRIGGERS
	pass

func _RemoveBomb(Bomb: TBomb) -> void:
	var playerID = Bomb.PlayerID
	Data.RemoveBomb(Bomb.ID)
	var idx = Data.FindSlotForPlayer(playerID)
	if idx != Constants.INVALID_SLOT:
		Data.Slots[idx].Player.DroppedBombs -= 1
	pass


func _RemoveExplosions(Delta: float) -> void:
	for field: Vector2i in Data.Explosions:
		var e = Data.Explosions[field]
		e.RemainingTime -= Delta
		if e.RemainingTime <= 0:
			Network.SendMapTileChanged.rpc(field, Maps.GetFieldType(Data.Map, field))
			Network.SendRemoveExplosion.rpc(field)
			Data.RemoveExplosion(field)
	pass

func _CreateExplosionAt(Field: Vector2i) -> bool:
	if Field.x < 0 || Field.y < 0 || Field.x >= Types.MAP_WIDTH || Field.y >= Types.MAP_HEIGHT:
		return false
	var f = Maps.GetFieldType(Data.Map, Field)
	if Data.FieldHasExplosion(Field):
		Data.ResetExplosionTime(Field)
		return true
	elif Tools.FieldCanExplode(f):
		if Data.FieldHasExplosion(Field):
			Data.ResetExplosionRemainingTime(Field)
			return true
		Data.AddExplosionAt(Field)
		Network.SendCreateExplosionAt.rpc(Field)
		_ExplodeStuffInField(Field, f)
		return f == Types.FIELD_EMPTY
	return false

func _ExplodeStuffInField(Field: Vector2i, FieldType: int) -> void:
	if FieldType == Types.FIELD_BRICK || Tools.FieldTypeIsPowerup(FieldType):
		var new_type = Types.FIELD_EMPTY
		if FieldType == Types.FIELD_BRICK:
			new_type = _RandomlySpawnPopup()
		Maps.SetFieldTypeTo(Data.Map, Field, new_type)
		Network.SendMapTileChanged.rpc(Field, Types.FIELD_EMPTY)
	var bomb = Data.GetBombInField(Field)
	if bomb:
		_ExplodeBomb(bomb)
	pass

func _CreateExplosionsInDirection(Field: Vector2i, Direction: Vector2i, Strength: int) -> void:
	var v: Vector2i = Field + Direction
	for i in range(Strength):
		if !_CreateExplosionAt(v):
			break
		v += Direction
	pass

func _CreateExplosionsForBomb(Bomb: TBomb) -> void:
	_CreateExplosionAt(Bomb.Position)
	_CreateExplosionsInDirection(Bomb.Position, Vector2i(-1, 0), Bomb.Strength)
	_CreateExplosionsInDirection(Bomb.Position, Vector2i(1, 0), Bomb.Strength)
	_CreateExplosionsInDirection(Bomb.Position, Vector2i(0, -1), Bomb.Strength)
	_CreateExplosionsInDirection(Bomb.Position, Vector2i(0, 1), Bomb.Strength)
	pass


func _KillPlayersInExplosions() -> void:
	var playerWasKilled: bool = false
	for i in Data.Slots.size():
		var slot = Data.Slots[i]
		if Data.FieldHasExplosion(slot.Player.Position):
			_KillPlayer(i)
			playerWasKilled = true
	if playerWasKilled:
		_CheckIfRoundEnded()
	pass


func _KillPlayer(SlotIndex: int) -> void:
	Data.Slots[SlotIndex].Player.Alive = false
	Network.SendPlayerDied.rpc(Data.Slots[SlotIndex].PlayerID)
	pass


func _RandomlySpawnPopup() -> int:
	var n = randi_range(0, Data.AvailablePowerups.size() + 1)
	if n < Data.AvailablePowerups.size():
		var pu: Types.TMapPowerUp = Data.AvailablePowerups[n]
		return Types.FIELD_PU_FIRST + pu.Number
	return Types.FIELD_EMPTY

func _PickUpPowerups() -> void:
	for i in Data.Slots.size():
		var slot: TSlot = Data.Slots[i]
		var type: int = Maps.GetFieldType(Data.Map, slot.Player.Position)
		if Tools.FieldTypeIsPowerup(type):
			Maps.SetFieldTypeTo(Data.Map, slot.Player.Position, Types.FIELD_EMPTY)
			Network.SendMapTileChanged.rpc(slot.Player.Position, Types.FIELD_EMPTY)
			Rules.ApplyPowerupToPlayer(Data, i, type - Types.FIELD_PU_FIRST)
			Network.SendPlayingState.rpc(slot.PlayerID, slot.Player.ToJSONString())
	pass


func _ProcessDiseases(delta: float) -> void:
	for i in Data.Slots.size():
		var slot: TSlot = Data.Slots[i]
		var state_changed: bool = false
		for idx in Constants.NUM_DISEASES:
			if slot.Player.Diseases[idx] > 0:
				slot.Player.Diseases[idx] -= delta
				if slot.Player.Diseases[idx] <= 0:
					slot.Player.Diseases[idx] = 0
					state_changed = true
		if state_changed:
			Network.SendPlayingState.rpc(slot.PlayerID, slot.Player.ToJSONString())
	pass


func _CheckIfRoundEnded() -> void:
	if Data.CountAlivePlayers() <= 1:
		if Data.CurrentRound == Data.NumRounds - 1:
			_EndMatch()
		else:
			_EndRound()
	pass


func _ConnectToSignalsOnStart() -> void:
	multiplayer.peer_connected.connect(_peer_connected)
	multiplayer.peer_disconnected.connect(_peer_disconnected)
	Network.OnRequestJoinLobby.connect(Comms._network_request_join_lobby)
	Network.OnLeaveLobby.connect(Comms._network_leave_lobby)
	Network.OnRequestMapChange.connect(Comms._network_request_map_change)
	Network.OnRequestMoveToSlot.connect(Comms._network_request_move_to_slot)
	Network.OnRequestStartMatch.connect(Comms._network_request_start_match)
	Network.OnPlayerReady.connect(Comms._network_player_ready)
	Network.OnPlayerHoldingKey.connect(Comms._network_player_is_holding_key)
	Network.OnRequestNumRounds.connect(Comms._network_request_num_rounds)
	Network.OnPlayerNameReceived.connect(Comms._network_player_name_received)
	Network.OnPlayerDirectionChange.connect(Comms._network_player_direction_change)
	Network.OnBombIsMoving.connect(Comms._network_bomb_is_moving)
	pass

func _DisconnectFromSignalsOnStop() -> void:
	multiplayer.peer_connected.disconnect(_peer_connected)
	multiplayer.peer_disconnected.disconnect(_peer_disconnected)
	Network.OnRequestJoinLobby.disconnect(Comms._network_request_join_lobby)
	Network.OnLeaveLobby.disconnect(Comms._network_leave_lobby)
	Network.OnRequestMapChange.disconnect(Comms._network_request_map_change)
	Network.OnRequestMoveToSlot.disconnect(Comms._network_request_move_to_slot)
	Network.OnRequestStartMatch.disconnect(Comms._network_request_start_match)
	Network.OnPlayerReady.disconnect(Comms._network_player_ready)
	Network.OnPlayerHoldingKey.disconnect(Comms._network_player_is_holding_key)
	Network.OnRequestNumRounds.disconnect(Comms._network_request_num_rounds)
	Network.OnPlayerNameReceived.disconnect(Comms._network_player_name_received)
	Network.OnPlayerDirectionChange.disconnect(Comms._network_player_direction_change)
	Network.OnBombIsMoving.disconnect(Comms._network_bomb_is_moving)
	pass


func _ClientDisconnected(SenderID) -> void:
	Data.ClearSlotForPlayer(SenderID)
	Network.SendPlayerLeftLobby.rpc(SenderID)
	pass


func _SendLobbyInfoToPlayer(PlayerID: int) -> void:
	if State == TState.LOBBY:
		for i in Data.Slots.size():
			var slot: TSlot = Data.Slots[i]
			var other_peerid: int = slot.PlayerID
			if (other_peerid != 0) && (other_peerid != PlayerID):
				Network.SendPlayerMovedToSlot.rpc_id(PlayerID, other_peerid, i)
				Network.SendPlayerNameChanged.rpc_id(PlayerID, other_peerid, slot.PlayerName)
		Network.SendMapName.rpc_id(PlayerID, CurrentMapName)
		Network.SendNumRoundsChanged.rpc(Data.NumRounds)
	pass


func _StartMatch() -> void:
	State = TState.MATCH
	Network.SendMatchStarted.rpc()
	
	Data.ResetPlayersBeforeMatch()
	Data.SetAllPlayersUnready()
	for i in Data.Slots.size():
		if Data.Slots[i].PlayerID != 0:
			Network.SendPlayerBecameReady.rpc(Data.Slots[i].PlayerID, false)
	Data.CurrentRound = -1
	
	_StartNewRound()
	pass


func _StartNewRound() -> void:
	Data.ResetPlayersBeforeRound()
	Data.ResetBombsEtcBeforeRound()
	Data.CountingDown = false
	Data.CurrentRound += 1
	Network.SendNewRound.rpc(CurrentMapName, Data.CurrentRound)
	pass


func _StartCountDownToRound() -> void:
	if OS.has_feature("debug"):
		Data.CountDownTime = 1.0
	else:
		Data.CountDownTime = COUNTDOWN_TIME
	Data.CountingDown = true
	Network.SendCountDownStarted.rpc(Data.CountDownTime)
	pass


func _StartRoundNow() -> void:
	Data.SetMap(Maps.LoadMap(CurrentMapName))
	var tempMap: Types.TMap = Maps.CopyMap(Data.Map)
	
	Data.SetPlayersToStartPositions()
	for slot in Data.Slots:
		if slot.PlayerID != 0:
			Maps.ClearFieldsAround(Data.Map, slot.Player.Position)
	Rules.ApplyInitialPowerupsToPlayers(Data)
	
	for y in Data.Map.Height:
		for x in Data.Map.Width:
			var type = Data.Map.Fields[y][x]
			if tempMap.Fields[y][x] != type:
				Network.SendMapTileChanged.rpc(Vector2i(x, y), type)
	
	for slot in Data.Slots:
		if slot.PlayerID != 0:
			Network.SendPlayerPosition.rpc(slot.PlayerID, slot.Player.Position)
			Network.SendPlayingState.rpc(slot.PlayerID, slot.Player.ToJSONString())
	
	State = TState.ROUND
	Network.SendRoundStarted.rpc()
	pass


func _EndRound() -> void:
	_IncreasePlayerScores()
	_SetAllPlayersUnready()
	State = TState.MATCH
	Network.SendRoundEnded.rpc()
	_StartNewRound()
	pass


func _EndMatch() -> void:
	_IncreasePlayerScores()
	_SetAllPlayersUnready()
	State = TState.LOBBY
	Network.SendMatchEnded.rpc()
	pass


func _SetAllPlayersUnready() -> void:
	Data.SetAllPlayersUnready()
	for idx in Data.Slots.size():
		if Data.Slots[idx].PlayerID != 0:
			Network.SendPlayerBecameReady.rpc(Data.Slots[idx].PlayerID, Data.Slots[idx].Ready)
	pass


func _IncreasePlayerScores() -> void:
	for slot: TSlot in Data.Slots:
		if slot.Player.Alive:
			slot.Score += 1
			Network.SendPlayerScore.rpc(slot.PlayerID, slot.Score)
	pass



func Start() -> bool:
	assert(!Network.IsConnected())
	if Network.IsConnected():
		return false
	
	assert(State == TState.IDLE)
	if State != TState.IDLE:
		return false
	
	var _peer = ENetMultiplayerPeer.new()

	var error = _peer.create_server(Network.PORT, Network.MAX_CLIENTS)
	if error != OK:
		_log("failed to start server: error=" + str(error))
		return false
	
	Data = TGameData.new()
	Data.InitSlots()
	
	Maps = TMaps.new()
	Maps.FindMapFiles()
	if Maps.MapNames.size() > 0:
		CurrentMapName = Maps.MapNames[0]
	
	Comms = TServerComms.new()
	Comms.Initialize(self)
	
	_ConnectToSignalsOnStart()
	Network.SetPeerTo(_peer)
	
	State = TState.LOBBY
	set_process(true)
	
	_log("server started")
	return true


func Stop() -> bool:
	assert(Network.IsServer())
	if !Network.IsServer():
		return false
	
	assert(State != TState.IDLE)
	if State == TState.IDLE:
		return false
	
	set_process(false)
	
	Network.ResetPeer()
	_DisconnectFromSignalsOnStop()
	
	Data.queue_free()
	Data = null
	Maps.queue_free()
	Maps = null
	CurrentMapName = ""
	Comms.queue_free()
	Comms = null
	
	State = TState.IDLE
	
	_log("server stopped")
	return true


func UpdatePlayerPosition(ID: int, Position: Vector2) -> void:
	var idx = Data.FindSlotForPlayer(ID)
	if idx != Constants.INVALID_SLOT:
		Data.Slots[idx].Player.Position = Position
	pass

func UpdateBombPosition(BombID: int, Position: Vector2) -> void:
	var bomb = Data.GetBombForID(BombID)
	if bomb:
		if bomb.IsMoving && !bomb.IsPunched:
			bomb.Position = Position
	pass


func ProcessDoubleTap(PlayerID, KeyIndex) -> void:
	match KeyIndex:
		Constants.DOUBLETAP_PRIMARY:
			var slot: TSlot = Data.GetSlotForPlayer(PlayerID)
			if slot:
				if slot.Player.PrimaryAction == Constants.PA_SPOOGER:
					var fields: Array[Vector2i] = _FindFieldsForSpooger(slot)
					for field: Vector2i in fields:
						if slot.Player.DroppedBombs < slot.Player.TotalBombs:
							_DropBomb(slot, field)
					pass
	pass

func _FindFieldsForSpooger(Slot: TSlot) -> Array[Vector2i]:
	var result: Array[Vector2i] = []
	var vector: Vector2i
	match Slot.Player.Direction:
		Constants.DIRECTION_LEFT: vector = Vector2i(-1, 0)
		Constants.DIRECTION_RIGHT: vector = Vector2i(1, 0)
		Constants.DIRECTION_UP: vector = Vector2i(0, -1)
		Constants.DIRECTION_DOWN: vector = Vector2i(0, 1)
	var field: Vector2i = Slot.Player.Position
	field += vector
	while Maps.GetFieldType(Data.Map, field) == Types.FIELD_EMPTY:
		result.append(field)
		field += vector 
	return result
