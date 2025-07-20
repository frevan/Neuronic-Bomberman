extends Node


signal OnConnectedToServer
signal OnDisconnectedFromServer
signal OnConnectionToServerFailed
signal OnLobbyJoined
signal OnLobbyRefused


func _connected_to_server() -> void:
	print(str(Network.PeerID) + " - connected to server")
	_DoStuffWhenConnected()
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
		print(str(Network.PeerID) + " - joined lobby")
		OnLobbyJoined.emit()
	else:
		print(str(Network.PeerID) + " - couldn't get into lobby")
		Disconnect()
		OnLobbyRefused.emit()
	pass


func _ConnectToSignals() -> void:
	multiplayer.connected_to_server.connect(_connected_to_server)
	multiplayer.connection_failed.connect(_connection_failed)
	multiplayer.server_disconnected.connect(_server_disconnected)
	Network.OnResponseToJoinLobby.connect(_network_response_to_join_lobby)
	pass


func _DisconnectFromSignals() -> void:
	multiplayer.connected_to_server.disconnect(_connected_to_server)
	multiplayer.connection_failed.disconnect(_connection_failed)
	multiplayer.server_disconnected.disconnect(_server_disconnected)
	Network.OnResponseToJoinLobby.disconnect(_network_response_to_join_lobby)
	pass


func _ConnectToServer(Address: String) -> bool:
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
	return true


func _DisconnectFromServer() -> bool:
	assert(!Server.IsRunning())
	if Server.IsRunning():
		return false
	
	Network.ResetPeer()
	return true


func _DoStuffWhenConnected() -> void:
	OnConnectedToServer.emit()
	Network.SendJoinLobby.rpc_id(1)
	pass


func Connect(Address: String) -> bool:
	if Network.IsServer():
		if Address != "127.0.0.1":
			return false
	else:
		if !_ConnectToServer(Address):
			return false
	
	print(str(Network.PeerID) + " - connect to server")
	
	_ConnectToSignals()
	
	if Network.IsServer():
		_DoStuffWhenConnected()
	
	return true


func Disconnect() -> bool:
	print(str(Network.PeerID) + " - disconnect from the server")
	
	assert(Server.IsConnected())
	if !Server.IsConnected():
		return false
	
	_DisconnectFromSignals()
	
	if !Network.IsServer():
		if !_DisconnectFromServer():
			return false
	return true
