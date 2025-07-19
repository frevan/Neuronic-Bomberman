extends Node

const PORT = 15063
const MAX_CLIENTS = 10

var peer: ENetMultiplayerPeer = null


func IsConnected() -> bool:
	return is_instance_valid(peer)


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

	var error = tempPeer.create_server(PORT, MAX_CLIENTS)
	if error != OK:
		print("failed to start server: error=" + str(error))
		return false
		
	multiplayer.multiplayer_peer = tempPeer
	peer = tempPeer
	print(str(multiplayer.get_unique_id()) + " - server started")
	return true



func Stop() -> bool:
	assert(IsRunning())
	if !IsRunning():
		return false
	
	var id = multiplayer.get_unique_id()
	multiplayer.multiplayer_peer.close()
	peer = null
	
	print(str(id) + " - server stopped")
	return true


func Connect(Address: String) -> bool:
	assert(!IsConnected())
	if (IsConnected()):
		return false
	
	var tempPeer = ENetMultiplayerPeer.new()
	
	var error = tempPeer.create_client(Address, PORT)
	if error != OK:
		return false
		
	tempPeer.get_peer(1).set_timeout(0, 0, 3000)
	multiplayer.multiplayer_peer = tempPeer
	peer = tempPeer
	
	print(str(multiplayer.get_unique_id()) + " - connect to server")
	return true


func Disconnect() -> bool:
	assert(IsConnected())
	if !IsConnected():
		return false
	assert(!IsRunning())
	if IsRunning():
		return false
	
	var id = multiplayer.get_unique_id()
	multiplayer.multiplayer_peer.close()
	peer = null
	
	print(str(id) + " - disconnected from the server")
	return true
