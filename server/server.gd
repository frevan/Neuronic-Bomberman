extends Node

class_name TServer


func _network_request_join_lobby(SenderID: int) -> void:
	Network.RespondToJoinLobby.rpc_id(SenderID, true)
	print(str(Network.PeerID) + " - request to join by: " + str(SenderID))
	pass


func IsConnected() -> bool:
	return is_instance_valid(Network.Peer)


func IsRunning() -> bool:
	if IsConnected():
		return multiplayer.is_server()
	else:
		return false


func ConnectToSignalsOnStart() -> void:
	Network.OnRequestJoinLobby.connect(_network_request_join_lobby)
	pass

func DisconnectFromSignalsOnStop() -> void:
	Network.OnRequestJoinLobby.disconnect(_network_request_join_lobby)
	pass


func Start() -> bool:
	assert(!IsConnected())
	if IsConnected():
		return false
	
	var _peer = ENetMultiplayerPeer.new()

	var error = _peer.create_server(Network.PORT, Network.MAX_CLIENTS)
	if error != OK:
		print("failed to start server: error=" + str(error))
		return false
	
	ConnectToSignalsOnStart()
	Network.SetPeerTo(_peer)
	
	print(str(Network.PeerID) + " - server started")
	return true



func Stop() -> bool:
	assert(IsRunning())
	if !IsRunning():
		return false
	
	var id = Network.PeerID
	Network.ResetPeer()
	DisconnectFromSignalsOnStop()
	
	print(str(id) + " - server stopped")
	return true
