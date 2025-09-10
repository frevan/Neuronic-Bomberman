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


func _network_player_is_holding_key(PlayerID: int, KeyIndex: int, Value: bool) -> void:
	var idx = Server.Data.FindSlotForPlayer(PlayerID)
	if idx != Constants.INVALID_SLOT:
		var slot: TSlot = Server.Data.Slots[idx]
		match KeyIndex:
			Constants.HOLDINGKEY_PRIMARY: 
				slot.Player.HoldingPrimaryKey = Value
				if !Value:
					slot.Player.CanGrabBombNow = true
			Constants.HOLDINGKEY_SECONDARY: 
				slot.Player.HoldingSecondaryKey = Value
			Constants.DOUBLETAP_PRIMARY: 
				Server.ProcessDoubleTap(PlayerID, KeyIndex)
	pass


func _network_request_win_condition(Condition: Constants.WinCondition, Value: int) -> void:
	_log("win condition request: " + str(Condition) + " with value " + str(Value))
	Server.Data.WinCondition = clamp(Condition, Constants.WinCondition.NUM_ROUNDS, Constants.WinCondition.SCORE)
	var v: int = 0
	match Server.Data.WinCondition:
		Constants.WinCondition.NUM_ROUNDS: 
			Server.Data.NumRounds = clamp(Value, Server.NUM_ROUNDS_MIN, Server.NUM_ROUNDS_MAX)
			v = Server.Data.NumRounds
		Constants.WinCondition.SCORE: 
			Server.Data.ScoreToWin = clamp(Value, Server.SCORE_MIN, Server.SCORE_MAX)
			v = Server.Data.ScoreToWin
	Network.SendWinConditionChanged.rpc(Server.Data.WinCondition, v)
	pass


func _network_player_name_received(PlayerID: int, Name: String) -> void:
	_log("player name received for " + str(PlayerID) + ": " + Name)
	var idx = Server.Data.FindSlotForPlayer(PlayerID)
	if idx != Constants.INVALID_SLOT:
		Server.Data.Slots[idx].PlayerName = Name
		Network.SendPlayerNameChanged.rpc(PlayerID, Name)
	pass


func _network_player_direction_change(PlayerID: int, NewDirection: int) -> void:
	#_log("player direction received for " + str(PlayerID) + ": " + str(NewDirection))
	var slot: TSlot = Server.Data.GetSlotForPlayer(PlayerID)
	if slot:
		slot.Player.Direction = NewDirection
	pass


func _network_bomb_is_moving(BombID: int, Value: bool) -> void:
	if Server.Data.Bombs.has(BombID):
		var bomb: TBomb = Server.Data.Bombs[BombID]
		bomb.IsMoving = Value
	pass
