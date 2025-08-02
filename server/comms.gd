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


func _network_request_map_change(MapName: String) -> void:
	_log("request to change the map to: " + MapName)
	if Server.Maps.MapExists(MapName):
		Server.CurrentMapName = MapName
		Network.SendMapName.rpc(Server.CurrentMapName)
	pass


func _network_request_move_to_slot(SenderID: int, SlotIndex: int) -> void:
	_log("request to move " + str(SenderID) + " to slot " + str(SlotIndex))
	
	var idx: int = Server.Data.FindSlotForPlayer(SenderID)
	if Server.State == TServer.TState.LOBBY:
		if Server.Data.MovePlayerToSlotIfFree(SenderID, idx):
			idx = SlotIndex
	
	Network.SendPlayerMovedToSlot.rpc(SenderID, idx)
	pass


func _network_request_start_match(SenderID: int) -> void:
	_log("request to start the match from " + str(SenderID))
	if (Server.State == TServer.TState.LOBBY) && (SenderID == 1):
		Server._StartMatch()
	pass


func _network_player_ready(SenderID: int, Ready: bool) -> void:
	_log("player " + str(SenderID) + " is ready: " + str(Ready))
	if (Server.State == TServer.TState.LOBBY) || (Server.State == TServer.TState.MATCH):
		Server.Data.SetPlayerReady(SenderID, Ready)
		Network.SendPlayerBecameReady.rpc(SenderID, Ready)
		if Server.State == TServer.TState.MATCH:
			if Server.Data.AreAllPlayersReady():
				Server._StartCountDownToRound()
	pass


func _network_player_is_dropping_bomb(PlayerID: int, Value: bool) -> void:
	var idx = Server.Data.FindSlotForPlayer(PlayerID)
	if idx != Constants.INVALID_SLOT:
		Server.Data.Slots[idx].DroppingBombs = Value
		if Server.Data.Slots[idx].DroppingBombs:
			Server._DropBomb(idx)
	pass


func _network_request_num_rounds(Value: int) -> void:
	_log("num rounds request: " + str(Value))
	Server.Data.NumRounds = clamp(Value, Server.NUM_ROUNDS_MIN, Server.NUM_ROUNDS_MAX)
	Network.SendNumRoundsChanged.rpc(Server.Data.NumRounds)
	pass


func _network_player_name_received(PlayerID: int, Name: String) -> void:
	_log("player name received for " + str(PlayerID) + ": " + Name)
	var idx = Server.Data.FindSlotForPlayer(PlayerID)
	if idx != Constants.INVALID_SLOT:
		Server.Data.Slots[idx].PlayerName = Name
		Network.SendPlayerNameChanged.rpc(PlayerID, Name)
	pass
