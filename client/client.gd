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
signal OnPlayerPositionChanged # params: player_id (int)
signal OnMapTileChanged # params: field (vector2i), type (int)


var Data: TGameData
var CurrentMapName: String = ""
@onready var Maps: TMaps = TMaps.new()

enum TState {IDLE, CONNECTING, LOBBY, MATCH}
var State: TState = TState.IDLE


func _log(Text: String) -> void:
	print(str(Network.PeerID) + " [client] " + Text)
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
	OnMatchStarted.emit()
	pass
	
	
func _network_new_round(MapName: String) -> void:
	_log("new round: " + MapName)
	
	Data.Map = Maps.LoadMap(MapName)
	if is_instance_valid(Data.Map):
		_log("Loaded map " + Data.Map.Name + " with caption: " + Data.Map.Caption)
		CurrentMapName = MapName
	else:
		# TODO: handle errors loading the map
		_log("failed to load map: " + MapName)
	
	Data.ResetPlayersBeforeRound()
	
	OnNewRound.emit()
	Network.SendPlayerReady.rpc_id(1, true)
	pass


func _network_player_became_ready(PlayerID: int, Ready: bool) -> void:
	_log("player " + str(PlayerID) + " is ready: " + str(Ready))
	pass


func _network_round_started() -> void:
	_log("round started")
	pass


func _network_player_position_received(PlayerID: int, Position: Vector2) -> void:
	_log("player " + str(PlayerID) + " is at: (" + str(Position.x) + "," + str(Position.y) + ")")
	var idx = Data.FindSlotForPlayer(PlayerID)
	if idx != Types.INVALID_SLOT:
		var p: Types.TPlayer = Data.Slots[idx].Player 
		p.Position = Position
		OnPlayerPositionChanged.emit(PlayerID)
	pass


func _network_map_tile_changed(Field: Vector2i, Type: int) -> void:
	_log("map tile " + str(Field) + " changed to " + str(Type))
	Maps.SetFieldTypeTo(Data.Map, Field, Type)
	OnMapTileChanged.emit(Field, Type)
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
