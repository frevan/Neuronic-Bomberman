extends Node

class_name TServer

const PORT = 15063
const MAX_CLIENTS = 10

var Peer: ENetMultiplayerPeer = null


func IsConnected() -> bool:
	return is_instance_valid(Peer)


func IsRunning() -> bool:
	if IsConnected() && (is_instance_valid(multiplayer)):
		return multiplayer.is_server()
	else:
		return false


func Start() -> bool:
	assert(!IsConnected())
	if IsConnected():
		return false
	
	var tempPeer = ENetMultiplayerPeer.new()

	var error = tempPeer.create_server(PORT, MAX_CLIENTS)
	if error != OK:
		print("failed to start server: error=" + str(error))
		return false
		
	multiplayer.multiplayer_peer = tempPeer
	Peer = tempPeer
	print(str(multiplayer.get_unique_id()) + " - server started")
	return true



func Stop() -> bool:
	assert(IsRunning())
	if !IsRunning():
		return false
	
	var id = multiplayer.get_unique_id()
	multiplayer.multiplayer_peer.close()
	Peer = null
	
	print(str(id) + " - server stopped")
	return true
