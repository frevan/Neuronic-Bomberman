extends Node

const PORT = 15063
const MAX_CLIENTS = 10

var peer = null


func IsConnected() -> bool:
	return is_instance_valid(peer)


func IsRunning() -> bool:
	if IsConnected():
		return multiplayer.is_server()
	else:
		return false


func Start() -> void:
	assert(!IsConnected())
	
	peer = ENetMultiplayerPeer.new()
	peer.create_server(PORT, MAX_CLIENTS)
	multiplayer.multiplayer_peer = peer
	
	print(str(multiplayer.get_unique_id()) + " - server started")
	pass


func Stop() -> void:
	assert(IsRunning())
	
	var id = multiplayer.get_unique_id()
	multiplayer.multiplayer_peer.close()
	peer = null
	
	print(str(id) + " - server stopped")
	pass


func Connect(Address: String) -> void:
	assert(!IsConnected())
	
	peer = ENetMultiplayerPeer.new()	
	peer.create_client(Address, PORT)
	peer.get_peer(1).set_timeout(0, 0, 3000)
	multiplayer.multiplayer_peer = peer
	
	print(str(multiplayer.get_unique_id()) + " - connect to server")
	pass


func Disconnect() -> void:
	assert(IsConnected())
	assert(!IsRunning())
	
	var id = multiplayer.get_unique_id()
	multiplayer.multiplayer_peer.close()
	peer = null
	
	print(str(id) + " - disconnected from the server")
	pass
