extends Node


signal OnConnectedToServer
signal OnDisconnectedFromServer
signal OnConnectionToServerFailed
signal OnLobbyJoined
signal OnLobbyRefused


func _connected_to_server() -> void:
	print(str(Network.PeerID) + " - connected to server")
	OnConnectedToServer.emit()
	Network.SendJoinLobby.rpc_id(1)
	pass


func _connection_failed() -> void:
	print(str(Network.PeerID) + " - failed to connect")
	Disconnect()
	OnConnectionToServerFailed.emit()
	pass


func _server_disconnected() -> void:
	print(str(Network.PeerID) + " - disconnected from server")
	Disconnect()
	OnDisconnectedFromServer.emit()
	pass


func _network_response_to_join_lobby(Accepted: bool) -> void:
	if Accepted:
		OnLobbyJoined.emit()
		print(str(Network.PeerID) + " - joined lobby")
	else:
		Disconnect()
		OnLobbyRefused.emit()
		print(str(Network.PeerID) + " - couldn't get into lobby")
	pass


func ConnectToSignalsOnConnect() -> void:
	multiplayer.connected_to_server.connect(_connected_to_server)
	multiplayer.connection_failed.connect(_connection_failed)
	multiplayer.server_disconnected.connect(_server_disconnected)
	Network.OnResponseToJoinLobby.connect(_network_response_to_join_lobby)
	pass


func DisconnectFromSignalsOnDisconnect() -> void:
	multiplayer.connected_to_server.disconnect(_connected_to_server)
	multiplayer.connection_failed.disconnect(_connection_failed)
	multiplayer.server_disconnected.disconnect(_server_disconnected)
	pass


func Connect(Address: String) -> bool:
	assert(!Server.IsConnected())
	if (Server.IsConnected()):
		return false
	assert(!Server.IsRunning())
	if Server.IsRunning():
		return false
	
	var _peer = ENetMultiplayerPeer.new()
	
	var error = _peer.create_client(Address, Network.PORT)
	if error != OK:
		return false
		
	Network.SetPeerTo(_peer)
	ConnectToSignalsOnConnect()
	
	print(str(Network.PeerID) + " - connect to server")
	return true


func Disconnect() -> bool:
	assert(Server.IsConnected())
	if !Server.IsConnected():
		return false
	assert(!Server.IsRunning())
	if Server.IsRunning():
		return false
	
	var id = Network.PeerID
	Network.ResetPeer()
	DisconnectFromSignalsOnDisconnect()
	
	print(str(id) + " - disconnected from the server")
	return true
