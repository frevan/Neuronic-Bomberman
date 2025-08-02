extends Node

class_name TServerComms


var Server: TServer = null


func Initialize(SetServer: TServer) -> void:
	Server = SetServer
	pass


func _log(Text: String) -> void:
	print(str(Network.PeerID) + " [srv] " + Text)
	pass


func _network_request_join_lobby(SenderID: int) -> void:
	_log("request to join by: " + str(SenderID))
	
	var success: bool = false
	var slot_idx: int = Constants.INVALID_SLOT
	
	if Server.State == TServer.TState.LOBBY:
		slot_idx = Server.Data.FindFreeSlotIndex()
		if slot_idx >= 0: 
			success = true
			Server.Data.Slots[slot_idx].PlayerID = SenderID
		Network.SendJoinLobbyResponse.rpc_id(SenderID, success)
		
	if success:
		Network.SendPlayerJoinedLobby.rpc(SenderID)
		Network.SendPlayerMovedToSlot.rpc(SenderID, slot_idx)
		Server._SendLobbyInfoToPlayer(SenderID)
	pass


func _network_leave_lobby(SenderID: int) -> void:
	_log("player " + str(SenderID) + " left the lobby")
	Server._ClientDisconnected(SenderID)
	pass
