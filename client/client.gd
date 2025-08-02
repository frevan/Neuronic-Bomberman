extends Node

class_name TClient


signal OnConnectedToServer
signal OnDisconnectedFromServer
signal OnConnectionToServerFailed
signal OnLobbyJoined
signal OnLobbyRefused
signal OnPlayerJoined # params: player_id (int)
signal OnPlayerLeft # params: player_id (int)
signal OnPlayerMovedToSlot # params: player_id (int), slot_index (int)
signal OnMapNameChanged # params: map_name (string)
signal OnMatchStarted
signal OnNewRound
signal OnRoundStarted
signal OnRoundEnded
signal OnMatchEnded
signal OnPlayerPositionChanged # params: player_id (int)
signal OnMapTileChanged # params: field (vector2i), type (int)
signal OnBombDropped # params: field (vector2i)
signal OnRemoveBomb # params: field (vector2i)
signal OnExplosion # params: field (vector2i)
signal OnRemoveExplosion # params: field (vector2i)
signal OnPlayerDied # params: id (int)
signal OnPlayerScoreChanged # params: id (int), score (int)
signal OnPlayerBecameReady # params: id (int), ready (bool)
signal OnCountDownStarted # params: time (float)
signal OnNumRoundsChanged # params: value (int)
signal OnPlayerDisconnected # params: id (int)
signal OnPlayerNameChanged # params: id (int), name (string)

signal OnPlayerPositionUpdated # [for the server object] param: id (int), position (vector2)


var PlayerName: String = ""
var Data: TGameData
var CurrentMapName: String = ""
@onready var Maps: TMaps = TMaps.new()

enum TState {IDLE, CONNECTING, LOBBY, MATCH, ROUND}
var State: TState = TState.IDLE


func _log(Text: String) -> void:
	print(str(Network.PeerID) + " [client] " + Text)
	pass


func _process(delta: float) -> void:
	if !Data:
		return
	if State == TState.MATCH || State == TState.ROUND:
		if Data.CountingDown:
			Data.CountDownTime -=  delta
			if Data.CountDownTime <= 0:
				Data.CountingDown = false
	pass


func _connected_to_server() -> void:
	_log("connected to server")
	if State == TState.CONNECTING:
		_DoStuffWhenConnected()
	pass


func _connection_failed() -> void:
	_log("failed to connect")
	if State == TState.CONNECTING:
		_DoStuffWhenDisconnected()
		OnConnectionToServerFailed.emit()
	pass


func _server_disconnected() -> void:
	_log("disconnected from server")
	if State != TState.IDLE:
		_DoStuffWhenDisconnected()
	pass


func _peer_disconnected(SenderID: int) -> void:
	_log("peer disconnected: " + str(SenderID))
	if (State != TState.IDLE) && is_instance_valid(Data):
		Data.ClearSlotForPlayer(SenderID)
		OnPlayerDisconnected.emit(SenderID)
	pass


func _network_response_to_join_lobby(Accepted: bool) -> void:
	if State == TState.CONNECTING:
		if Accepted:
			_log("joined lobby")
			State = TState.LOBBY
			OnLobbyJoined.emit()
		else:
			_log("couldn't get into lobby")
			Disconnect()
			OnLobbyRefused.emit()
	pass


func _network_player_joined_lobby(PlayerID: int) -> void:
	_log("player " + str(PlayerID) + " joined the lobby")
	if State != TState.IDLE:
		OnPlayerJoined.emit(PlayerID)
	pass


func _network_player_left_lobby(PlayerID: int) -> void:
	_log("player " + str(PlayerID) + " left the lobby")
	if State != TState.IDLE:
		Data.ClearSlotForPlayer(PlayerID)
		OnPlayerLeft.emit(PlayerID)
	pass


func _network_player_moved_to_slot(PlayerID: int, SlotIndex: int) -> void:
	assert(PlayerID != 0)
	assert(SlotIndex < Data.Slots.size())
	
	_log("player " + str(PlayerID) + " moved to slot " + str(SlotIndex))
	
	if (State != TState.IDLE) && (SlotIndex != Types.INVALID_SLOT):
		Data.MovePlayerToSlot(PlayerID, SlotIndex)
		OnPlayerMovedToSlot.emit(PlayerID, SlotIndex)
		Network.SendPlayerName.rpc_id(1, PlayerName)
	pass


func _network_map_changed(MapName: String) -> void:
	_log("map changed to " + MapName)
	if State != TState.IDLE:
		CurrentMapName = MapName
		OnMapNameChanged.emit(CurrentMapName)
	pass


func _network_match_started() -> void:
	_log("match started")
	State = TState.MATCH
	Data.ResetPlayersBeforeMatch()
	OnMatchStarted.emit()
	pass
	
	
func _network_new_round(MapName: String, Round: int) -> void:
	_log("new round #" + str(Round) + ": " + MapName)
	
	Data.SetMap(Maps.LoadMap(MapName))
	if is_instance_valid(Data.Map):
		_log("Loaded map " + Data.Map.Name + " with caption: " + Data.Map.Caption)
		CurrentMapName = MapName
	else:
		# TODO: handle errors loading the map
		_log("failed to load map: " + MapName)
	
	Data.ResetPlayersBeforeRound()
	Data.ResetBombsEtcBeforeRound()
	Data.CurrentRound = Round
	
	OnNewRound.emit()
	pass


func _network_round_started() -> void:
	_log("round started")
	State = TState.ROUND
	Data.CountingDown = false
	OnRoundStarted.emit()
	pass


func _network_round_ended() -> void:
	_log("round ended")
	State = TState.MATCH
	OnRoundEnded.emit()
	pass


func _network_match_ended() -> void:
	_log("match ended")
	State = TState.LOBBY
	OnMatchEnded.emit()
	pass


func _network_player_became_ready(PlayerID: int, Ready: bool) -> void:
	_log("player " + str(PlayerID) + " is ready: " + str(Ready))
	var idx = Data.FindSlotForPlayer(PlayerID)
	if idx != Types.INVALID_SLOT:
		Data.Slots[idx].Ready = Ready
		OnPlayerBecameReady.emit(PlayerID, Ready)
	pass


func _network_player_position_received(PlayerID: int, Position: Vector2) -> void:
	_log("player " + str(PlayerID) + " is at: (" + str(Position.x) + "," + str(Position.y) + ")")
	var idx = Data.FindSlotForPlayer(PlayerID)
	if idx != Types.INVALID_SLOT:
		Data.Slots[idx].Position = Position
		OnPlayerPositionChanged.emit(PlayerID)
	pass


func _network_map_tile_changed(Field: Vector2i, Type: int) -> void:
	_log("map tile " + str(Field) + " changed to " + str(Type))
	Maps.SetFieldTypeTo(Data.Map, Field, Type)
	OnMapTileChanged.emit(Field, Type)
	pass


func _network_bomb_dropped(Field: Vector2i)-> void:
	_log("bomb dropped on field " + str(Field))
	Data.AddBombAt(Field, 0)
	OnBombDropped.emit(Field)
	pass


func _network_create_explosion_at(Field: Vector2i) -> void:
	_log("explosion at " + str(Field))
	Data.RemoveBomb(Field)
	OnRemoveBomb.emit(Field)
	Data.AddExplosionAt(Field)
	OnExplosion.emit(Field)
	pass


func _network_remove_explosion(Field: Vector2i) -> void:
	_log("no more explosion at " + str(Field))
	Data.RemoveExplosion(Field)
	OnRemoveExplosion.emit(Field)
	pass


func _network_player_died(PlayerID: int) -> void:
	_log("player " + str(PlayerID) + " died")
	var slot_idx = Data.FindSlotForPlayer(PlayerID)
	if slot_idx != Types.INVALID_SLOT:
		Data.Slots[slot_idx].Alive = false
	OnPlayerDied.emit(PlayerID)
	pass


func _network_update_player_score(PlayerID: int, Score: int) -> void:
	_log("player " + str(PlayerID) + "'s score is now " + str(Score))
	Data.UpdatePlayerScore(PlayerID, Score)
	OnPlayerScoreChanged.emit(PlayerID, Score)
	pass


func _network_countdown_started(CountDownTime: float) -> void:
	_log("countdown started: " + str(CountDownTime) + " seconds")
	Data.CountingDown = true
	Data.CountDownTime = CountDownTime
	OnCountDownStarted.emit(CountDownTime)
	pass


func _network_num_rounds_changed(Value: int) -> void:
	_log("Number of rounds changed to " + str(Value))
	Data.NumRounds = Value
	OnNumRoundsChanged.emit(Value)
	pass


func _network_player_name_changed(PlayerID: int, Name: String) -> void:
	_log("Player " + str(PlayerID) + " is now called " + Name)
	var idx = Data.FindSlotForPlayer(PlayerID)
	if idx != Types.INVALID_SLOT:
		Data.Slots[idx].PlayerName = Name
		OnPlayerNameChanged.emit(PlayerID, Name)
	pass


func _ConnectToSignals() -> void:
	multiplayer.connected_to_server.connect(_connected_to_server)
	multiplayer.connection_failed.connect(_connection_failed)
	multiplayer.server_disconnected.connect(_server_disconnected)
	multiplayer.peer_disconnected.connect(_peer_disconnected)
	Network.OnResponseToJoinLobby.connect(_network_response_to_join_lobby)
	Network.OnPlayerJoinedLobby.connect(_network_player_joined_lobby)
	Network.OnPlayerLeftLobby.connect(_network_player_left_lobby)
	Network.OnPlayerMovedToSlot.connect(_network_player_moved_to_slot)
	Network.OnMapChanged.connect(_network_map_changed)
	Network.OnMatchStarted.connect(_network_match_started)
	Network.OnNewRound.connect(_network_new_round)
	Network.OnPlayerBecameReady.connect(_network_player_became_ready)
	Network.OnRoundStarted.connect(_network_round_started)
	Network.OnPlayerPositionReceived.connect(_network_player_position_received)
	Network.OnMapTileChanged.connect(_network_map_tile_changed)
	Network.OnBombDropped.connect(_network_bomb_dropped)
	Network.OnCreateExplosionAt.connect(_network_create_explosion_at)
	Network.OnRemoveExplosion.connect(_network_remove_explosion)
	Network.OnPlayerDied.connect(_network_player_died)
	Network.OnRoundEnded.connect(_network_round_ended)
	Network.OnMatchEnded.connect(_network_match_ended)
	Network.OnUpdatePlayerScore.connect(_network_update_player_score)
	Network.OnCountDownStarted.connect(_network_countdown_started)
	Network.OnNumRoundsChanged.connect(_network_num_rounds_changed)
	Network.OnPlayerNameChanged.connect(_network_player_name_changed)
	pass


func _DisconnectFromSignals() -> void:
	multiplayer.connected_to_server.disconnect(_connected_to_server)
	multiplayer.connection_failed.disconnect(_connection_failed)
	multiplayer.server_disconnected.disconnect(_server_disconnected)
	multiplayer.peer_disconnected.disconnect(_peer_disconnected)
	Network.OnResponseToJoinLobby.disconnect(_network_response_to_join_lobby)
	Network.OnPlayerJoinedLobby.disconnect(_network_player_joined_lobby)
	Network.OnPlayerLeftLobby.disconnect(_network_player_left_lobby)
	Network.OnPlayerMovedToSlot.disconnect(_network_player_moved_to_slot)
	Network.OnMapChanged.disconnect(_network_map_changed)
	Network.OnMatchStarted.disconnect(_network_match_started)
	Network.OnNewRound.disconnect(_network_new_round)
	Network.OnPlayerBecameReady.disconnect(_network_player_became_ready)
	Network.OnRoundStarted.disconnect(_network_round_started)
	Network.OnPlayerPositionReceived.disconnect(_network_player_position_received)
	Network.OnMapTileChanged.disconnect(_network_map_tile_changed)
	Network.OnBombDropped.disconnect(_network_bomb_dropped)
	Network.OnCreateExplosionAt.disconnect(_network_create_explosion_at)
	Network.OnRemoveExplosion.disconnect(_network_remove_explosion)
	Network.OnPlayerDied.disconnect(_network_player_died)
	Network.OnRoundEnded.disconnect(_network_round_ended)
	Network.OnMatchEnded.disconnect(_network_match_ended)
	Network.OnUpdatePlayerScore.disconnect(_network_update_player_score)
	Network.OnCountDownStarted.disconnect(_network_countdown_started)
	Network.OnNumRoundsChanged.disconnect(_network_num_rounds_changed)
	Network.OnPlayerNameChanged.disconnect(_network_player_name_changed)
	pass


func _DoStuffWhenConnected() -> void:
	OnConnectedToServer.emit()
	Network.SendJoinLobby.rpc_id(1)
	pass


func _DoStuffWhenDisconnected() -> void:
	_DisconnectFromSignals()
	
	State = TState.IDLE
	OnDisconnectedFromServer.emit()
	
	if !Network.IsServer():
		Network.ResetPeer()
		
	Data = null
	CurrentMapName = ""
	pass


func Connect(Address: String) -> bool:
	assert(State == TState.IDLE)
	if State != TState.IDLE:
		return false
	
	if Network.IsServer():
		if Address != "127.0.0.1":
			return false
	else:
		if !Network.ConnectToServerAsClient(Address):
			return false
	
	_log("connect to server")
	
	Data = TGameData.new()
	Data.InitSlots()
	
	_ConnectToSignals()
	
	State = TState.CONNECTING
	
	if Network.IsServer():
		_DoStuffWhenConnected()
	return true


func Disconnect() -> bool:
	_log("disconnect from the server")
	
	assert(Network.IsConnected())
	if !Network.IsConnected():
		return false
	
	assert(State != TState.IDLE)
	if State == TState.IDLE:
		return false
	
	Network.SendLeaveLobby.rpc_id(1)
	_DoStuffWhenDisconnected()
	return true


func MovePlayerToSlot(SlotIndex: int) -> bool:
	_log("move to slot: " + str(SlotIndex))
	
	assert(Network.IsConnected())
	if !Network.IsConnected():
		return false
	
	Network.SendMovePlayerToSlot.rpc_id(1, SlotIndex)
	return true


func RequestMapChange(MapName: String) -> void:
	if Network.IsServer() && (State == TState.LOBBY):
		Network.SendRequestMapChange.rpc_id(1, MapName)
	pass


func RequestStartMatch() -> void:
	if Network.IsServer() && (State == TState.LOBBY):
		Network.SendStartMatch.rpc_id(1)
	pass


func DropBombs(IsDropping: bool) -> void:
	Network.SendDroppingBombs.rpc_id(1, IsDropping)
	pass


func UpdatePlayerPosition(ID: int, Position: Vector2) -> void:
	var idx = Data.FindSlotForPlayer(ID)
	if idx != Types.INVALID_SLOT:
		Data.Slots[idx].Position = Position
		if Network.PeerID == 1:
			OnPlayerPositionUpdated.emit(ID, Position)
	pass


func RequestPlayerReady(Ready: bool) -> void:
	Network.SendPlayerReady.rpc_id(1, Ready)
	pass


func RequestNumRounds(Value: int) -> void:
	Network.SendRequestNumRounds.rpc_id(1, Value)
	pass
