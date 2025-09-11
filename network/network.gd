extends Node

class_name TNetwork


# received by server only
signal OnRequestJoinLobby # params: sender_id (int)
signal OnLeaveLobby # params: sender_id (int)
signal OnRequestMoveToSlot # params: sender_id (int), slot_idx (int)
signal OnRequestMapChange # params: map_nake (string)
signal OnRequestStartMatch # params: sender_id (int)
signal OnPlayerReady # params: sender_id (int), ready (bool)
signal OnPlayerHoldingKey # params: sender_id (int), key_index (int), value (bool)
signal OnRequestWinCondition(Condition: Constants.WinCondition, Value: int)
signal OnRequestMaxTime(Value: int)
signal OnPlayerNameReceived # params: id (int), name (string)
signal OnPlayerDirectionChange # params: id (int), direction (int)
signal OnBombIsMoving(BombID: int, Value: bool)
# received by clients
signal OnResponseToJoinLobby # params: accepted (bool)
signal OnPlayerJoinedLobby # params: player_id (int)
signal OnPlayerLeftLobby # params: player_id (int)
signal OnPlayerMovedToSlot # params: player_id (int), slot_index (int)
signal OnMapChanged # params: map_name (string)
signal OnMatchStarted
signal OnNewRound # params: map_name (string), round # (int)
signal OnPlayerBecameReady # params: player_id (int), ready (bool)
signal OnRoundStarted
signal OnRoundEnded
signal OnMatchEnded
signal OnPlayerPositionReceived # params: player_id (int), field (vector2)
signal OnMapTileChanged # params: field (vector2i), type (int)
signal OnBombDropped # params: id (int), bomb id (int), type (int), position (vector2)
signal OnBombStatus(BombID: int, Status: String) # Status is a JSON string
signal OnBombPosition(BombID: int, Position: Vector2)
signal OnCreateExplosionAt # params: field (vector2i)
signal OnRemoveExplosion # params: field (vector2i)
signal OnPlayerDied # params: id (int)
signal OnUpdatePlayerScore # params: id (int), score (int)
signal OnCountDownStarted # params: time (float)
signal OnWinConditionChanged(Condition: Constants.WinCondition, Value: int)
signal OnMaxTimeChanged(Value: int)
signal OnPlayerNameChanged # params: id (int), value (string)
signal OnPlayingStateUpdate # params: id (int), state (JSON string)
signal OnPlayerGrabbedPowerup(PlayerID: int, PowerupType: int)


const PORT = 15063
const MAX_CLIENTS = 10
const CONNECTION_TIMEOUT = 3000

var Peer: ENetMultiplayerPeer
var PeerID: int


func IsConnected() -> bool:
	return is_instance_valid(Peer)


func IsServer() -> bool:
	return IsConnected() && (PeerID == 1)


func SetPeerTo(NewValue: ENetMultiplayerPeer) -> void:
	assert(is_instance_valid(NewValue))
	
	Peer = NewValue
	PeerID = Peer.get_unique_id()
	if PeerID != 1:
		Peer.get_peer(1).set_timeout(0, 0, CONNECTION_TIMEOUT)
	multiplayer.multiplayer_peer = Peer
	pass


func ResetPeer() -> void:
	assert(is_instance_valid(Peer))
	
	multiplayer.multiplayer_peer = null
	Peer = null
	PeerID = 0
	pass


func ConnectToServerAsClient(Address: String) -> bool:
	assert(!IsConnected())
	if (IsConnected()):
		return false
	assert(!IsServer())
	if IsServer():
		return false
	
	var _peer = ENetMultiplayerPeer.new()
	
	var error = _peer.create_client(Address, PORT)
	if error != OK:
		return false
		
	SetPeerTo(_peer)
	return true


@rpc("reliable", "call_local", "any_peer")
func SendJoinLobby() -> void:
	var id = multiplayer.get_remote_sender_id()
	OnRequestJoinLobby.emit(id)

@rpc("reliable", "call_local", "any_peer")
func SendLeaveLobby() -> void:
	var id = multiplayer.get_remote_sender_id()
	OnLeaveLobby.emit(id)

@rpc("reliable", "call_local", "any_peer")
func SendMovePlayerToSlot(SlotIndex: int) -> void:
	var id = multiplayer.get_remote_sender_id()
	OnRequestMoveToSlot.emit(id, SlotIndex)

@rpc("reliable", "call_local", "authority")
func SendRequestMapChange(MapName: String) -> void:
	OnRequestMapChange.emit(MapName)

@rpc("reliable", "call_local", "authority")
func SendStartMatch() -> void:
	var id = multiplayer.get_remote_sender_id()
	OnRequestStartMatch.emit(id)

@rpc("reliable", "call_local", "any_peer")
func SendPlayerReady(Ready: bool) -> void:
	var id = multiplayer.get_remote_sender_id()
	OnPlayerReady.emit(id, Ready)

@rpc("reliable", "call_local", "any_peer")
func SendHoldingKey(KeyIndex: int, Value: bool) -> void:
	var id = multiplayer.get_remote_sender_id()
	OnPlayerHoldingKey.emit(id, KeyIndex, Value)

@rpc("reliable", "call_local", "authority")
func SendRequestWinCondition(Condition: Constants.WinCondition, Value: int) -> void:
	OnRequestWinCondition.emit(Condition, Value)

@rpc("reliable", "call_local", "authority")
func SendRequestMaxTime(Value: int) -> void:
	OnRequestMaxTime.emit(Value)

@rpc("reliable", "call_local", "any_peer")
func SendPlayerName(Value: String) -> void:
	var id = multiplayer.get_remote_sender_id()
	OnPlayerNameReceived.emit(id, Value)

@rpc("reliable", "call_local", "any_peer")
func SendPlayerDirection(Value: int) -> void:
	var id = multiplayer.get_remote_sender_id()
	OnPlayerDirectionChange.emit(id, Value)

@rpc("reliable", "call_local", "any_peer")
func SendBombIsMoving(BombID: int, Value: bool) -> void:
	if multiplayer.get_remote_sender_id() == 1:
		OnBombIsMoving.emit(BombID, Value)


@rpc("reliable", "call_local", "authority")
func SendJoinLobbyResponse(Accepted: bool) -> void:
	OnResponseToJoinLobby.emit(Accepted)

@rpc("reliable", "call_local", "authority")
func SendPlayerJoinedLobby(ID: int) -> void:
	OnPlayerJoinedLobby.emit(ID)

@rpc("reliable", "call_local", "authority")
func SendPlayerLeftLobby(ID: int) -> void:
	OnPlayerLeftLobby.emit(ID)

@rpc("reliable", "call_local", "authority")
func SendPlayerMovedToSlot(ID: int, SlotIndex: int) -> void:
	OnPlayerMovedToSlot.emit(ID, SlotIndex)

@rpc("reliable", "call_local", "authority")
func SendMapName(MapName: String) -> void:
	OnMapChanged.emit(MapName)

@rpc("reliable", "call_local", "authority")
func SendMatchStarted() -> void:
	OnMatchStarted.emit()

@rpc("reliable", "call_local", "authority")
func SendNewRound(MapName: String, Round: int) -> void:
	OnNewRound.emit(MapName, Round)

@rpc("reliable", "call_local", "authority")
func SendRoundStarted() -> void:
	OnRoundStarted.emit()

@rpc("reliable", "call_local", "authority")
func SendRoundEnded() -> void:
	OnRoundEnded.emit()

@rpc("reliable", "call_local", "authority")
func SendMatchEnded() -> void:
	OnMatchEnded.emit()

@rpc("reliable", "call_local", "authority")
func SendPlayerBecameReady(ID: int, Ready: bool) -> void:
	OnPlayerBecameReady.emit(ID, Ready)

@rpc("reliable", "call_local", "authority")
func SendPlayerPosition(ID: int, Field: Vector2) -> void:
	OnPlayerPositionReceived.emit(ID, Field)

@rpc("reliable", "call_local", "authority")
func SendMapTileChanged(Field: Vector2i, Type: int) -> void:
	OnMapTileChanged.emit(Field, Type)

@rpc("reliable", "call_local", "authority")
func SendBombDropped(ID: int, BombID: int, Type: int, Position: Vector2) -> void:
	OnBombDropped.emit(ID, BombID, Type, Position)

@rpc("reliable", "call_local", "authority")
func SendBombStatus(BombID: int, Status: String) -> void:
	OnBombStatus.emit(BombID, Status)

@rpc("reliable", "call_local", "authority")
func SendBombPosition(BombID: int, Position: Vector2) -> void:
	OnBombPosition.emit(BombID, Position)

@rpc("reliable", "call_local", "authority")
func SendCreateExplosionAt(Field: Vector2i) -> void:
	OnCreateExplosionAt.emit(Field)

@rpc("reliable", "call_local", "authority")
func SendRemoveExplosion(Field: Vector2i) -> void:
	OnRemoveExplosion.emit(Field)

@rpc("reliable", "call_local", "authority")
func SendPlayerDied(ID: int) -> void:
	OnPlayerDied.emit(ID)

@rpc("reliable", "call_local", "authority")
func SendPlayerScore(ID: int, Score: int) -> void:
	OnUpdatePlayerScore.emit(ID, Score)

@rpc("reliable", "call_local", "authority")
func SendCountDownStarted(CountDownTime: float) -> void:
	OnCountDownStarted.emit(CountDownTime)

@rpc("reliable", "call_local", "authority")
func SendWinConditionChanged(Condition: Constants.WinCondition, Value: int) -> void:
	OnWinConditionChanged.emit(Condition, Value)

@rpc("reliable", "call_local", "authority")
func SendMaxTimeChanged(Value: int) -> void:
	OnMaxTimeChanged.emit(Value)

@rpc("reliable", "call_local", "authority")
func SendPlayerNameChanged(ID: int, Value: String) -> void:
	OnPlayerNameChanged.emit(ID, Value)

@rpc("reliable", "call_local", "authority")
func SendPlayingState(ID: int, State: String) -> void:
	OnPlayingStateUpdate.emit(ID, State)

@rpc("reliable", "call_local", "authority")
func SendPlayerGrabbedPowerup(ID: int, PowerupType: int) -> void:
	OnPlayerGrabbedPowerup.emit(ID, PowerupType)
