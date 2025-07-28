extends Node

class_name TServer


const COUNTDOWN_TIME = 5

const NUM_ROUNDS_MIN = 1
const NUM_ROUNDS_MAX = 100

var Data: TGameData
var Maps: TMaps
var CurrentMapName: String

enum TState {IDLE, LOBBY, MATCH, ROUND}
var State: TState = TState.IDLE


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
		if Data.CountingDown:
			Data.CountDownTime -=  delta
			if Data.CountDownTime <= 0:
				Data.CountingDown = false
				_StartRoundNow()
	elif State == TState.ROUND:
		_ExplodeBombs(delta)
		_RemoveExplosions(delta)
		_KillPlayersInExplosions()
	pass


func _peer_connected(SenderID: int) -> void:
	_log("peer connected: " + str(SenderID))
	pass


func _peer_disconnected(SenderID: int) -> void:
	_log("peer disconnected: " + str(SenderID))
	_ClientDisconnected(SenderID)
	pass


func _network_request_join_lobby(SenderID: int) -> void:
	_log("request to join by: " + str(SenderID))
	
	var success: bool = false
	var slot_idx: int = Types.INVALID_SLOT
	
	if State == TState.LOBBY:
		slot_idx = Data.FindFreeSlotIndex()
		if slot_idx >= 0: 
			success = true
			Data.Slots[slot_idx].PlayerID = SenderID
		Network.SendJoinLobbyResponse.rpc_id(SenderID, success)
		
	if success:
		Network.SendPlayerJoinedLobby.rpc(SenderID)
		Network.SendPlayerMovedToSlot.rpc(SenderID, slot_idx)
		_SendLobbyInfoToPlayer(SenderID)
	pass


func _network_leave_lobby(SenderID: int) -> void:
	_log("player " + str(SenderID) + " left the lobby")
	_ClientDisconnected(SenderID)
	pass


func _network_request_map_change(MapName: String) -> void:
	_log("request to change the map to: " + MapName)
	if Maps.MapExists(MapName):
		CurrentMapName = MapName
		Network.SendMapName.rpc(CurrentMapName)
	pass


func _network_request_move_to_slot(SenderID: int, SlotIndex: int) -> void:
	_log("request to move " + str(SenderID) + " to slot " + str(SlotIndex))
	
	var idx: int = Data.FindSlotForPlayer(SenderID)
	if State == TState.LOBBY:
		if Data.MovePlayerToSlotIfFree(SenderID, idx):
			idx = SlotIndex
	
	Network.SendPlayerMovedToSlot.rpc(SenderID, idx)
	pass


func _network_request_start_match(SenderID: int) -> void:
	_log("request to start the match from " + str(SenderID))
	if (State == TState.LOBBY) && (SenderID == 1):
		_StartMatch()
	pass


func _network_player_ready(SenderID: int, Ready: bool) -> void:
	_log("player " + str(SenderID) + " is ready: " + str(Ready))
	if (State == TState.LOBBY) || (State == TState.MATCH):
		Data.SetPlayerReady(SenderID, Ready)
		Network.SendPlayerBecameReady.rpc(SenderID, Ready)
		if State == TState.MATCH:
			if Data.AreAllPlayersReady():
				_StartCountDownToRound()
	pass


func _network_player_is_dropping_bomb(PlayerID: int, Value: bool) -> void:
	var idx = Data.FindSlotForPlayer(PlayerID)
	if idx != Types.INVALID_SLOT:
		Data.Slots[idx].DroppingBombs = Value
		if Data.Slots[idx].DroppingBombs:
			_DropBomb(idx)
	pass


func _network_request_num_rounds(Value: int) -> void:
	_log("num rounds request: " + str(Value))
	Data.NumRounds = clamp(Value, NUM_ROUNDS_MIN, NUM_ROUNDS_MAX)
	Network.SendNumRoundsChanged.rpc(Data.NumRounds)
	pass


func _network_player_name_received(PlayerID: int, Name: String) -> void:
	_log("player name received for " + str(PlayerID) + ": " + Name)
	var idx = Data.FindSlotForPlayer(PlayerID)
	if idx != Types.INVALID_SLOT:
		Data.Slots[idx].PlayerName = Name
		Network.SendPlayerNameChanged.rpc(PlayerID, Name)
	pass


func _DropBomb(SlotIndex: int) -> void:
	var slot = Data.Slots[SlotIndex]
	if slot.DroppedBombs == slot.TotalBombs:
		return
	slot.DroppedBombs += 1
	var pos: Vector2i = slot.Position
	Data.AddBombAt(pos, slot.PlayerID)
	Network.SendBombDropped.rpc(pos)
	pass


func _ExplodeBombs(Delta: float) -> void:
	for field: Vector2i in Data.Bombs:
		var b = Data.Bombs[field]
		b.TimeUntilExplosion -= Delta
		if b.TimeUntilExplosion <= 0:
			_CreateExplosionsForBomb(b)
			Data.RemoveBomb(field)
			var slot_idx = Data.FindSlotForPlayer(b.PlayerID)
			if slot_idx != Types.INVALID_SLOT:
				Data.Slots[slot_idx].DroppedBombs -= 1
	pass


func _RemoveExplosions(Delta: float) -> void:
	for field: Vector2i in Data.Explosions:
		var e = Data.Explosions[field]
		e.RemainingTime -= Delta
		if e.RemainingTime <= 0:
			Network.SendRemoveExplosion.rpc(field)
			Data.RemoveExplosion(field)
	pass



func _CreateExplosionAt(Field: Vector2i) -> bool:
	if Field.x < 0 || Field.y < 0 || Field.x >= Types.MAP_WIDTH || Field.y >= Types.MAP_HEIGHT:
		return false
	var f = Data.Map.Fields[Field.y][Field.x]
	if f == Types.FIELD_EMPTY || f == Types.FIELD_BRICK:
		Data.AddExplosionAt(Field)
		Network.SendCreateExplosionAt.rpc(Field)
	if f == Types.FIELD_BRICK:
		Network.SendMapTileChanged.rpc(Field, Types.FIELD_EMPTY)
	return f == Types.FIELD_EMPTY

func _CreateExplosionsInDirection(Field: Vector2i, Direction: Vector2i, Strength: int) -> void:
	var v: Vector2i = Field + Direction
	for i in range(Strength):
		if !_CreateExplosionAt(v):
			break
		v += Direction
	pass


func _CreateExplosionsForBomb(Bomb: Types.TBomb) -> void:
	_CreateExplosionAt(Bomb.Field)
	_CreateExplosionsInDirection(Bomb.Field, Vector2i(-1, 0), Bomb.Strength)
	_CreateExplosionsInDirection(Bomb.Field, Vector2i(1, 0), Bomb.Strength)
	_CreateExplosionsInDirection(Bomb.Field, Vector2i(0, -1), Bomb.Strength)
	_CreateExplosionsInDirection(Bomb.Field, Vector2i(0, 1), Bomb.Strength)
	pass


func _KillPlayersInExplosions() -> void:
	var playerWasKilled: bool = false
	for i in Data.Slots.size():
		var slot = Data.Slots[i]
		if Data.FieldHasExplosion(slot.Position):
			_KillPlayer(i)
			playerWasKilled = true
	if playerWasKilled:
		_CheckIfRoundEnded()
	pass


func _KillPlayer(SlotIndex: int) -> void:
	Data.Slots[SlotIndex].Alive = false
	Network.SendPlayerDied.rpc(Data.Slots[SlotIndex].PlayerID)
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
	Network.OnRequestJoinLobby.connect(_network_request_join_lobby)
	Network.OnLeaveLobby.connect(_network_leave_lobby)
	Network.OnRequestMapChange.connect(_network_request_map_change)
	Network.OnRequestMoveToSlot.connect(_network_request_move_to_slot)
	Network.OnRequestStartMatch.connect(_network_request_start_match)
	Network.OnPlayerReady.connect(_network_player_ready)
	Network.OnPlayerIsDroppingBombs.connect(_network_player_is_dropping_bomb)
	Network.OnRequestNumRounds.connect(_network_request_num_rounds)
	Network.OnPlayerNameReceived.connect(_network_player_name_received)
	pass

func _DisconnectFromSignalsOnStop() -> void:
	multiplayer.peer_connected.disconnect(_peer_connected)
	multiplayer.peer_disconnected.disconnect(_peer_disconnected)
	Network.OnRequestJoinLobby.disconnect(_network_request_join_lobby)
	Network.OnLeaveLobby.disconnect(_network_leave_lobby)
	Network.OnRequestMapChange.disconnect(_network_request_map_change)
	Network.OnRequestMoveToSlot.disconnect(_network_request_move_to_slot)
	Network.OnRequestStartMatch.disconnect(_network_request_start_match)
	Network.OnPlayerReady.disconnect(_network_player_ready)
	Network.OnPlayerIsDroppingBombs.disconnect(_network_player_is_dropping_bomb)
	Network.OnRequestNumRounds.disconnect(_network_request_num_rounds)
	Network.OnPlayerNameReceived.disconnect(_network_player_name_received)
	pass


func _ClientDisconnected(SenderID) -> void:
	Data.ClearSlotForPlayer(SenderID)
	Network.SendPlayerLeftLobby.rpc(SenderID)
	pass


func _SendLobbyInfoToPlayer(PlayerID: int) -> void:
	if State == TState.LOBBY:
		for i in Data.Slots.size():
			var slot: Types.TSlot = Data.Slots[i]
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
	Data.Map = Maps.LoadMap(CurrentMapName)
	var tempMap: Types.TMap = Maps.CopyMap(Data.Map)
	
	Data.SetPlayersToStartPositions()
	for slot in Data.Slots:
		if slot.PlayerID != 0:
			Network.SendPlayerPosition.rpc(slot.PlayerID, slot.Position)
			Maps.ClearFieldsAround(Data.Map, slot.Position)
	
	for y in Data.Map.Height:
		for x in Data.Map.Width:
			var type = Data.Map.Fields[y][x]
			if tempMap.Fields[y][x] != type:
				Network.SendMapTileChanged.rpc(Vector2i(x, y), type)
	
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
	for slot: Types.TSlot in Data.Slots:
		if slot.Alive:
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
	
	State = TState.IDLE
	
	_log("server stopped")
	return true


func UpdatePlayerPosition(ID: int, Position: Vector2) -> void:
	var idx = Data.FindSlotForPlayer(ID)
	if idx != Types.INVALID_SLOT:
		Data.Slots[idx].Position = Position
	pass
