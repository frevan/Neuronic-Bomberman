extends Node

class_name TServer


var TGameData = preload("res://data/gamedata.gd")
var Data: TGameData


func _peer_connected(SenderID: int) -> void:
	print(str(Network.PeerID) + " - peer connected: " + str(SenderID))
	pass


func _peer_disconnected(SenderID: int) -> void:
	print(str(Network.PeerID) + " - peer disconnected: " + str(SenderID))
	_ClientDisconnected(SenderID)
	pass


func _network_request_join_lobby(SenderID: int) -> void:
	print(str(Network.PeerID) + " - request to join by: " + str(SenderID))
	var success: bool = false
	var slot_idx: int = Data.FindFreeSlotIndex()
	if slot_idx >= 0: 
		success = true
		Data.Slots[slot_idx].Player.PeerID = SenderID
	Network.SendJoinLobbyResponse.rpc_id(SenderID, success)
	if success:
		Network.SendPlayerMovedToSlot.rpc(SenderID, slot_idx)
		_SendInfoForAllPlayersToPlayer(SenderID)
	pass


func _network_request_move_to_slot(SenderID: int, SlotIndex: int) -> void:
	print(str(Network.PeerID) + " - request to move " + str(SenderID) + " to slot " + str(SlotIndex))
	var idx: int = SlotIndex
	if !Data.MovePlayerToSlotIfFree(SenderID, idx):
		idx = Data.FindSlotForPlayer(SenderID)
	Network.SendPlayerMovedToSlot.rpc(SenderID, idx)
	pass


func _ConnectToSignalsOnStart() -> void:
	multiplayer.peer_connected.connect(_peer_connected)
	multiplayer.peer_disconnected.connect(_peer_disconnected)
	Network.OnRequestJoinLobby.connect(_network_request_join_lobby)
	Network.OnRequestMoveToSlot.connect(_network_request_move_to_slot)
	pass

func _DisconnectFromSignalsOnStop() -> void:
	multiplayer.peer_connected.disconnect(_peer_connected)
	multiplayer.peer_disconnected.disconnect(_peer_disconnected)
	Network.OnRequestJoinLobby.disconnect(_network_request_join_lobby)
	Network.OnRequestMoveToSlot.disconnect(_network_request_move_to_slot)
	pass


func _ClientDisconnected(SenderID) -> void:
	Data.ClearSlotForPlayer(SenderID)
	pass


func _SendInfoForAllPlayersToPlayer(SenderID: int) -> void:
	for i in Data.Slots.size():
		var other_peerid: int = Data.Slots[i].Player.PeerID
		if (other_peerid != 0) && (other_peerid != SenderID):
			Network.SendPlayerMovedToSlot.rpc_id(SenderID, other_peerid, i)
	pass


func Start() -> bool:
	assert(!Network.IsConnected())
	if Network.IsConnected():
		return false
	
	var _peer = ENetMultiplayerPeer.new()

	var error = _peer.create_server(Network.PORT, Network.MAX_CLIENTS)
	if error != OK:
		print("failed to start server: error=" + str(error))
		return false
	
	Data = TGameData.new()
	Data.InitSlots()
	
	_ConnectToSignalsOnStart()
	Network.SetPeerTo(_peer)
	
	print(str(Network.PeerID) + " - server started")
	return true



func Stop() -> bool:
	assert(Network.IsServer())
	if !Network.IsServer():
		return false
	
	var id = Network.PeerID
	Network.ResetPeer()
	_DisconnectFromSignalsOnStop()
	
	Data.queue_free()
	Data = null
	
	print(str(id) + " - server stopped")
	return true
