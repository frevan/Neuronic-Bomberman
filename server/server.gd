extends Node

class_name TServer


var Data: TGameData
var Maps: TMaps
var CurrentMapName: String

enum TState {IDLE, LOBBY, MATCH, ROUND}
var State: TState = TState.IDLE


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
	var slot_idx: int = Types.INVALID_SLOT
	
	if State == TState.LOBBY:
		slot_idx = Data.FindFreeSlotIndex()
		if slot_idx >= 0: 
			success = true
			Data.Slots[slot_idx].Player.PeerID = SenderID
		Network.SendJoinLobbyResponse.rpc_id(SenderID, success)
		
	if success:
		Network.SendPlayerMovedToSlot.rpc(SenderID, slot_idx)
		_SendLobbyInfoToPlayer(SenderID)
	pass


func _network_leave_lobby(SenderID: int) -> void:
	print(str(Network.PeerID) + " - player " + str(SenderID) + " left the lobby")
	_ClientDisconnected(SenderID)
	pass


func _network_request_move_to_slot(SenderID: int, SlotIndex: int) -> void:
	print(str(Network.PeerID) + " - request to move " + str(SenderID) + " to slot " + str(SlotIndex))
	
	var idx: int = Data.FindSlotForPlayer(SenderID)
	if State == TState.LOBBY:
		if Data.MovePlayerToSlotIfFree(SenderID, idx):
			idx = SlotIndex
	
	Network.SendPlayerMovedToSlot.rpc(SenderID, idx)
	pass


func _network_request_start_match(SenderID: int) -> void:
	print(str(Network.PeerID) + " - request to start the match from " + str(SenderID))
	if (State == TState.LOBBY) && (SenderID == 1):
		_StartMatch()
	pass


func _network_player_ready(SenderID: int, Ready: bool) -> void:
	print(str(Network.PeerID) + " - player " + str(SenderID) + " is ready: " + str(Ready))
	if (State == TState.LOBBY) || (State == TState.MATCH):
		Data.SetPlayerReady(SenderID, Ready)
		Network.SendPlayerBecameReady.rpc(SenderID, Ready)
		#TODO: send a round started if we're waiting to start one
	pass


func _ConnectToSignalsOnStart() -> void:
	multiplayer.peer_connected.connect(_peer_connected)
	multiplayer.peer_disconnected.connect(_peer_disconnected)
	Network.OnRequestJoinLobby.connect(_network_request_join_lobby)
	Network.OnLeaveLobby.connect(_network_leave_lobby)
	Network.OnRequestMoveToSlot.connect(_network_request_move_to_slot)
	Network.OnRequestStartMatch.connect(_network_request_start_match)
	Network.OnPlayerReady.connect(_network_player_ready)
	pass

func _DisconnectFromSignalsOnStop() -> void:
	multiplayer.peer_connected.disconnect(_peer_connected)
	multiplayer.peer_disconnected.disconnect(_peer_disconnected)
	Network.OnRequestJoinLobby.disconnect(_network_request_join_lobby)
	Network.OnLeaveLobby.disconnect(_network_leave_lobby)
	Network.OnRequestMoveToSlot.disconnect(_network_request_move_to_slot)
	Network.OnRequestStartMatch.disconnect(_network_request_start_match)
	Network.OnPlayerReady.disconnect(_network_player_ready)
	pass


func _ClientDisconnected(SenderID) -> void:
	Data.ClearSlotForPlayer(SenderID)
	Network.SendPlayerLeftLobby.rpc(SenderID)
	pass


func _SendLobbyInfoToPlayer(PlayerID: int) -> void:
	if State == TState.LOBBY:
		for i in Data.Slots.size():
			var other_peerid: int = Data.Slots[i].Player.PeerID
			if (other_peerid != 0) && (other_peerid != PlayerID):
				Network.SendPlayerMovedToSlot.rpc_id(PlayerID, other_peerid, i)
		Network.SendMapName.rpc_id(PlayerID, CurrentMapName)
	pass


func _StartMatch() -> void:
	State = TState.MATCH
	Network.SendMatchStarted.rpc()
	Network.SendNewRound.rpc(CurrentMapName)
	pass


func Start() -> bool:
	assert(!Network.IsConnected())
	if Network.IsConnected():
		return false
	
	assert(State == TState.IDLE)
	if State != TState.IDLE:
		return false
	
	var _peer = ENetMultiplayerPeer.new()

	var error = _peer.create_server(Network.PORT, Network.MAX_CLIENTS)
	if error != OK:
		print("failed to start server: error=" + str(error))
		return false
	
	Data = TGameData.new()
	Data.InitSlots()
	
	Maps = TMaps.new()
	Maps.FindMapFiles()
	if Maps.MapNames.size() > 0:
		CurrentMapName = Maps.MapNames[0]
	
	_ConnectToSignalsOnStart()
	Network.SetPeerTo(_peer)
	
	State = TState.LOBBY
	
	print(str(Network.PeerID) + " - server started")
	return true



func Stop() -> bool:
	assert(Network.IsServer())
	if !Network.IsServer():
		return false
	
	assert(State != TState.IDLE)
	if State == TState.IDLE:
		return false
	
	var id = Network.PeerID
	Network.ResetPeer()
	_DisconnectFromSignalsOnStop()
	
	Data.queue_free()
	Data = null
	Maps.queue_free()
	Maps = null
	CurrentMapName = ""
	
	State = TState.IDLE
	
	print(str(id) + " - server stopped")
	return true
