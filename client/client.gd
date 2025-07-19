extends Node


signal OnConnectedToServer
signal OnDisconnectedFromServer
signal OnConnectionToServerFailed


var PeerID = 0


func _ready() -> void:
	multiplayer.connected_to_server.connect(_connected_to_server)
	multiplayer.connection_failed.connect(_connection_failed)
	multiplayer.server_disconnected.connect(_server_disconnected)
	pass


func _connected_to_server() -> void:
	print(str(PeerID) + " - connected to server")
	OnConnectedToServer.emit()
	pass
	
	
func _connection_failed() -> void:
	print(str(PeerID) + " - failed to connect")
	Disconnect()
	OnConnectionToServerFailed.emit()
	pass
	
	
func _server_disconnected() -> void:
	print(str(PeerID) + " - disconnected from server")
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
	
	var error = tempPeer.create_client(Address, Server.PORT)
	if error != OK:
		return false
		
	tempPeer.get_peer(1).set_timeout(0, 0, 3000)
	multiplayer.multiplayer_peer = tempPeer
	Server.Peer = tempPeer
	
	PeerID = Server.Peer.get_unique_id()
	
	print(str(PeerID) + " - connect to server")
	return true


func Disconnect() -> bool:
	assert(Server.IsConnected())
	if !Server.IsConnected():
		return false
	assert(!Server.IsRunning())
	if Server.IsRunning():
		return false
	
	var id = PeerID
	multiplayer.multiplayer_peer.close()
	Server.Peer = null
	PeerID = 0
	
	print(str(id) + " - disconnected from the server")
	return true
