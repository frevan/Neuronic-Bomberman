extends Node

class_name TClient


signal OnConnectedToServer
signal OnDisconnectedFromServer
signal OnConnectionToServerFailed
signal OnLobbyJoined
signal OnLobbyRefused


var Data: TGameData
var CurrentMapName: String = ""

enum TState {IDLE, CONNECTING, LOBBY, MATCH}
var State: TState = TState.IDLE


func _connected_to_server() -> void:
	print(str(Network.PeerID) + " - connected to server")
	if State == TState.CONNECTING:
		_DoStuffWhenConnected()
	pass


func _connection_failed() -> void:
	print(str(Network.PeerID) + " - failed to connect")
	if State == TState.CONNECTING:
		_DoStuffWhenDisconnected()
		OnConnectionToServerFailed.emit()
	pass


func _server_disconnected() -> void:
	print(str(Network.PeerID) + " - disconnected from server")
	if State != TState.IDLE:
		_DoStuffWhenDisconnected()
	pass


func _peer_disconnected(SenderID: int) -> void:
	print(str(Network.PeerID) + " - peer disconnected: " + str(SenderID))
	if (State != TState.IDLE) && is_instance_valid(Data):
		Data.ClearSlotForPlayer(SenderID)
	pass


func _network_response_to_join_lobby(Accepted: bool) -> void:
	if State == TState.CONNECTING:
		if Accepted:
			print(str(Network.PeerID) + " - joined lobby")
			State = TState.LOBBY
			OnLobbyJoined.emit()
		else:
			print(str(Network.PeerID) + " - couldn't get into lobby")
			Disconnect()
			OnLobbyRefused.emit()
	pass


func _network_player_left_lobby(PlayerID: int) -> void:
	print(str(Network.PeerID) + " - player " + str(PlayerID) + " left the lobby")
	if State != TState.IDLE:
		Data.ClearSlotForPlayer(PlayerID)
	pass


func _network_player_moved_to_slot(PlayerID: int, SlotIndex: int) -> void:
	assert(PlayerID != 0)
	assert(SlotIndex < Data.Slots.size())
	
	print(str(Network.PeerID) + " - player " + str(PlayerID) + " moved to slot " + str(SlotIndex))
	
	if (State != TState.IDLE) && (SlotIndex != Types.INVALID_SLOT):
		Data.MovePlayerToSlot(PlayerID, SlotIndex)
	pass


func _network_map_changed(MapName: String) -> void:
	print(str(Network.PeerID) + " - map changed to " + MapName)
	if State != TState.IDLE:
		CurrentMapName = MapName
	pass


func _network_match_started() -> void:
	print(str(Network.PeerID) + " - match started")
	State = TState.MATCH
	pass
	
	
func _network_new_round(MapName: String) -> void:
	print(str(Network.PeerID) + " - new round: " + MapName)
	# TODO: load map
	Network.SendPlayerReady(true)
	pass


func _network_player_became_ready(PlayerID: int, Ready: bool) -> void:
	print(str(Network.PeerID) + " - player " + str(PlayerID) + " is ready: " + str(Ready))
	pass


func _ConnectToSignals() -> void:
	multiplayer.connected_to_server.connect(_connected_to_server)
	multiplayer.connection_failed.connect(_connection_failed)
	multiplayer.server_disconnected.connect(_server_disconnected)
	multiplayer.peer_disconnected.connect(_peer_disconnected)
	Network.OnResponseToJoinLobby.connect(_network_response_to_join_lobby)
	Network.OnPlayerLeftLobby.connect(_network_player_left_lobby)
	Network.OnPlayerMovedToSlot.connect(_network_player_moved_to_slot)
	Network.OnMapChanged.connect(_network_map_changed)
	Network.OnMatchStarted.connect(_network_match_started)
	Network.OnNewRound.connect(_network_new_round)
	Network.OnPlayerBecameReady.connect(_network_player_became_ready)
	pass


func _DisconnectFromSignals() -> void:
	multiplayer.connected_to_server.disconnect(_connected_to_server)
	multiplayer.connection_failed.disconnect(_connection_failed)
	multiplayer.server_disconnected.disconnect(_server_disconnected)
	multiplayer.peer_disconnected.disconnect(_peer_disconnected)
	Network.OnResponseToJoinLobby.disconnect(_network_response_to_join_lobby)
	Network.OnPlayerLeftLobby.disconnect(_network_player_left_lobby)
	Network.OnPlayerMovedToSlot.disconnect(_network_player_moved_to_slot)
	Network.OnMapChanged.disconnect(_network_map_changed)
	Network.OnMatchStarted.disconnect(_network_match_started)
	Network.OnNewRound.disconnect(_network_new_round)
	Network.OnPlayerBecameReady.disconnect(_network_player_became_ready)
	pass


func _DoStuffWhenConnected() -> void:
	OnConnectedToServer.emit()
	Network.SendJoinLobby.rpc_id(1)
	pass


func _DoStuffWhenDisconnected() -> void:
	State = TState.IDLE	
	Network.SendLeaveLobby.rpc_id(1)
	OnDisconnectedFromServer.emit()
	
	if !Network.IsServer():
		Network.ResetPeer()
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
	
	print(str(Network.PeerID) + " - connect to server")
	
	Data = TGameData.new()
	Data.InitSlots()
	
	_ConnectToSignals()
	
	State = TState.CONNECTING
	
	if Network.IsServer():
		_DoStuffWhenConnected()
	return true


func Disconnect() -> bool:
	print(str(Network.PeerID) + " - disconnect from the server")
	
	assert(Network.IsConnected())
	if !Network.IsConnected():
		return false
	
	assert(State != TState.IDLE)
	if State == TState.IDLE:
		return false
	
	_DisconnectFromSignals()
	
	Data = null
	CurrentMapName = ""
	
	_DoStuffWhenDisconnected()
	return true


func MovePlayerToSlot(SlotIndex: int) -> bool:
	print(str(Network.PeerID) + " - move to slot: " + str(SlotIndex))
	
	assert(Network.IsConnected())
	if !Network.IsConnected():
		return false
	
	Network.SendMovePlayerToSlot.rpc_id(1, SlotIndex)
	
	return true
