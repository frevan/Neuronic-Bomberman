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
	
	var _peer = ENetMultiplayerPeer.new()

	var error = _peer.create_server(Network.PORT, Network.MAX_CLIENTS)
	if error != OK:
		print("failed to start server: error=" + str(error))
		return false
		
	Network.SetPeerTo(_peer)
	
	print(str(multiplayer.get_unique_id()) + " - server started")
	return true



func Stop() -> bool:
	assert(IsRunning())
	if !IsRunning():
		return false
	
	var id = Network.PeerID
	Network.ResetPeer()
	
	print(str(id) + " - server stopped")
	return true
