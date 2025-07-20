extends Node

class_name TServer


var TServerData = preload("res://server/server_data.gd")
var Data: TServerData


func _network_request_join_lobby(SenderID: int) -> void:
	print(str(Network.PeerID) + " - request to join by: " + str(SenderID))
	var success: bool = false
	var slot_idx: int = Data.FindFreeSlotIndex()
	if slot_idx >= 0: 
		success = true
		Data.Slots[slot_idx].Player.PeerID = SenderID
	Network.SendJoinLobbyResponse.rpc_id(SenderID, success)
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
	
	Data = TServerData.new()
	Data.InitSlots()
	
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
	
	Data.queue_free()
	Data = null
	
	print(str(id) + " - server stopped")
	return true
