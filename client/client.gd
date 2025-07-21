extends Node


signal OnConnectedToServer
signal OnDisconnectedFromServer
signal OnConnectionToServerFailed
signal OnLobbyJoined
signal OnLobbyRefused


var TGameData = preload("res://data/gamedata.gd")
var Data: TGameData

var CurrentMapName: String = ""


func _connected_to_server() -> void:
	print(str(Network.PeerID) + " - connected to server")
	_DoStuffWhenConnected()
	pass


func _connection_failed() -> void:
	print(str(Network.PeerID) + " - failed to connect")
	_DoStuffWhenDisconnected()
	OnConnectionToServerFailed.emit()
	pass


func _server_disconnected() -> void:
	print(str(Network.PeerID) + " - disconnected from server")
	_DoStuffWhenDisconnected()
	pass


func _peer_disconnected(SenderID: int) -> void:
	print(str(Network.PeerID) + " - peer disconnected: " + str(SenderID))
	if is_instance_valid(Data):
		Data.ClearSlotForPlayer(SenderID)
	pass


func _network_response_to_join_lobby(Accepted: bool) -> void:
	if Accepted:
		print(str(Network.PeerID) + " - joined lobby")
		OnLobbyJoined.emit()
	else:
		print(str(Network.PeerID) + " - couldn't get into lobby")
		Disconnect()
		OnLobbyRefused.emit()
	pass


func _network_player_left_lobby(PlayerID: int) -> void:
	print(str(Network.PeerID) + " - player " + str(PlayerID) + " left the lobby")
	Data.ClearSlotForPlayer(PlayerID)
	pass


func _network_player_moved_to_slot(PlayerID: int, SlotIndex: int) -> void:
	assert(PlayerID != 0)
	assert(SlotIndex < Data.Slots.size())
	
	print(str(Network.PeerID) + " - player " + str(PlayerID) + " moved to slot " + str(SlotIndex))
	
	if SlotIndex != Types.INVALID_SLOT:
		Data.MovePlayerToSlot(PlayerID, SlotIndex)
	pass


func _network_map_changed(MapName: String) -> void:
	print(str(Network.PeerID) + " - map changed to " + MapName)
	CurrentMapName = MapName
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
	pass


func _ConnectToServer(Address: String) -> bool:
	assert(!Network.IsConnected())
	if (Network.IsConnected()):
		return false
	assert(!Network.IsServer())
	if Network.IsServer():
		return false
	
	var _peer = ENetMultiplayerPeer.new()
	
	var error = _peer.create_client(Address, Network.PORT)
	if error != OK:
		return false
		
	Network.SetPeerTo(_peer)
	return true


func _DoStuffWhenConnected() -> void:
	OnConnectedToServer.emit()
	Network.SendJoinLobby.rpc_id(1)
	pass


func _DoStuffWhenDisconnected() -> void:
	Network.SendLeaveLobby.rpc_id(1)
	
	OnDisconnectedFromServer.emit()
	
	if !Network.IsServer():
		Network.ResetPeer()
	pass


func Connect(Address: String) -> bool:
	if Network.IsServer():
		if Address != "127.0.0.1":
			return false
	else:
		if !_ConnectToServer(Address):
			return false
	
	print(str(Network.PeerID) + " - connect to server")
	
	Data = TGameData.new()
	Data.InitSlots()
	
	_ConnectToSignals()
	
	if Network.IsServer():
		_DoStuffWhenConnected()
	return true


func Disconnect() -> bool:
	print(str(Network.PeerID) + " - disconnect from the server")
	
	assert(Network.IsConnected())
	if !Network.IsConnected():
		return false
	
	_DisconnectFromSignals()
	Data = null
	
	_DoStuffWhenDisconnected()
	return true


func MovePlayerToSlot(SlotIndex: int) -> bool:
	print(str(Network.PeerID) + " - move to slot: " + str(SlotIndex))
	
	assert(Network.IsConnected())
	if !Network.IsConnected():
		return false
	
	Network.SendMovePlayerToSlot.rpc_id(1, SlotIndex)
	
	return true
