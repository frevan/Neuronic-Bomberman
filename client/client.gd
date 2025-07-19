extends Node


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
	
	print(str(multiplayer.get_unique_id()) + " - connect to server")
	return true


func Disconnect() -> bool:
	assert(Server.IsConnected())
	if !Server.IsConnected():
		return false
	assert(!Server.IsRunning())
	if Server.IsRunning():
		return false
	
	var id = multiplayer.get_unique_id()
	multiplayer.multiplayer_peer.close()
	Server.Peer = null
	
	print(str(id) + " - disconnected from the server")
	return true
