extends Node

const PORT = 15063
const MAX_CLIENTS = 10

var peer = null


func IsServerRunning() -> bool:
	return is_instance_valid(peer)


func StartServer() -> void:
	assert(!is_instance_valid(peer))
	
	peer = ENetMultiplayerPeer.new()
	peer.create_server(PORT, MAX_CLIENTS)
	multiplayer.multiplayer_peer = peer
	
	print(str(multiplayer.get_unique_id()) + " - server started")
	pass


func StopServer() -> void:
	assert(is_instance_valid(peer))
	
	var id = multiplayer.get_unique_id()
	multiplayer.multiplayer_peer.close()
	peer = null
	
	print(str(id) + " - server stopped")
	pass
