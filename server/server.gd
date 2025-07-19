extends Node

class_name TServer


func IsConnected() -> bool:
	return is_instance_valid(Network.Peer)


func IsRunning() -> bool:
	if IsConnected():
		return multiplayer.is_server()
	else:
		return false


func Start() -> bool:
	assert(!IsConnected())
	if IsConnected():
		return false
	
	var tempPeer = ENetMultiplayerPeer.new()

	var error = tempPeer.create_server(Network.PORT, Network.MAX_CLIENTS)
	if error != OK:
		print("failed to start server: error=" + str(error))
		return false
		
	multiplayer.multiplayer_peer = tempPeer
	Network.Peer = tempPeer
	assert(is_instance_valid(Network.Peer))
	
	print(str(multiplayer.get_unique_id()) + " - server started")
	return true



func Stop() -> bool:
	assert(IsRunning())
	if !IsRunning():
		return false
	
	var id = multiplayer.get_unique_id()
	multiplayer.multiplayer_peer.close()
	Network.Peer = null
	assert(!is_instance_valid(Network.Peer))
	
	print(str(id) + " - server stopped")
	return true
