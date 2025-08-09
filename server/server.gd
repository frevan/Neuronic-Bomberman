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
		_ProcessPlayerKeys(delta)
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


func _ProcessPlayerKeys(_Delta: float) -> void:
	for i in Data.Slots.size():
		var slot: TSlot = Data.Slots[i]
		if (slot.PlayerID == 0) or !slot.Player.Alive:
			continue
		if slot.Player.HoldingPrimaryKey || slot.Player.Diseases[Constants.DISEASE_DIARRHEA]:
			_DropBomb(slot)
		if slot.Player.HoldingSecundaryKey:
			_ExplodeTriggerBombs(slot)
	pass

func _DropBomb(Slot: TSlot) -> void:
	var will_drop = false
	if Slot.Player.Diseases[Constants.DISEASE_DIARRHEA] > 0:
		will_drop = true
	else:
		var total = Slot.Player.TotalBombs
		var dropped = Slot.Player.DroppedBombs
		var constipated = Slot.Player.Diseases[Constants.DISEASE_CONSTIPATION] > 0
		will_drop = (dropped < total) && !constipated
		
	if will_drop:
		var pos: Vector2i = Slot.Player.Position
		var id: int = Tools.NewBombID()
		var type = Constants.BOMB_NORMAL
		if Slot.Player.NumTriggerBombs > 0:
			type = Constants.BOMB_TRIGGER
			Slot.Player.NumTriggerBombs -= 1
		if Data.AddBombAt(Slot.PlayerID, id, type, pos):
			Slot.Player.DroppedBombs += 1
			Network.SendBombDropped.rpc(Slot.PlayerID, id, type, pos)
	pass

func _ExplodeTriggerBombs(Slot: TSlot) -> void:
	var bomb: TBomb = Data.FindOldestTriggerBombFor(Slot.PlayerID)
	if bomb:
		_ExplodeBomb(bomb)
	pass


func _ProcessBombs(Delta: float) -> void:
	for id in Data.Bombs:
		var bomb: TBomb = Data.Bombs[id]
		bomb.TimeUntilExplosion -= Delta
		bomb.TimeSinceDrop += Delta
		if (bomb.Type == Constants.BOMB_NORMAL) && (bomb.TimeUntilExplosion <= 0):
			_ExplodeBomb(bomb)
	pass


func _ExplodeBomb(Bomb: TBomb) -> void:
	_RemoveBomb(Bomb)
	_CreateExplosionsForBomb(Bomb)
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
