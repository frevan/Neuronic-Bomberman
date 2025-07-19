extends Node


signal OnConnectedToServer
signal OnDisconnectedFromServer
signal OnConnectionToServerFailed


func _ready() -> void:
	pass


func _connected_to_server() -> void:
	print(str(Network.PeerID) + " - connected to server")
	OnConnectedToServer.emit()
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


func Connect(Address: String) -> bool:
	assert(!Server.IsConnected())
	if (Server.IsConnected()):
		return false
	assert(!Server.IsRunning())
	if Server.IsRunning():
		return false
	
	var tempPeer = ENetMultiplayerPeer.new()
	
	var error = tempPeer.create_client(Address, Network.PORT)
	if error != OK:
		return false
		
	tempPeer.get_peer(1).set_timeout(0, 0, 3000)
	multiplayer.multiplayer_peer = tempPeer
	Network.Peer = tempPeer
	assert(is_instance_valid(Network.Peer))
	Network.PeerID = Network.Peer.get_unique_id()
	
	multiplayer.connected_to_server.connect(_connected_to_server)
	multiplayer.connection_failed.connect(_connection_failed)
	multiplayer.server_disconnected.connect(_server_disconnected)
	
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
	multiplayer.multiplayer_peer.close()
	Network.Peer = null
	assert(!is_instance_valid(Network.Peer))
	Network.PeerID = 0
	
	multiplayer.connected_to_server.disconnect(_connected_to_server)
	multiplayer.connection_failed.disconnect(_connection_failed)
	multiplayer.server_disconnected.disconnect(_server_disconnected)
	
	print(str(id) + " - disconnected from the server")
	return true
