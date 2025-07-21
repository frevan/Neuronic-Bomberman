extends Node

class_name TNetwork


# received by server only
signal OnRequestJoinLobby # params: sender_id (int)
signal OnLeaveLobby # params: sender_id (int)
signal OnRequestMoveToSlot # params: sender_id (int), slot_idx (int)
# received by clients
signal OnResponseToJoinLobby # params: accepted (bool)
signal OnPlayerLeftLobby # params: player_id (int)
signal OnPlayerMovedToSlot # params: player_id (int), slot_index (int)
signal OnMapChanged # params: map_name (string)


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
	pass


@rpc("reliable", "call_local", "any_peer")
func SendLeaveLobby() -> void:
	var id = multiplayer.get_remote_sender_id()
	OnLeaveLobby.emit(id)
	pass


@rpc("reliable", "call_local", "any_peer")
func SendMovePlayerToSlot(SlotIndex: int) -> void:
	var id = multiplayer.get_remote_sender_id()
	OnRequestMoveToSlot.emit(id, SlotIndex)
	pass


@rpc("reliable", "call_local", "authority")
func SendJoinLobbyResponse(Accepted: bool) -> void:
	OnResponseToJoinLobby.emit(Accepted)
	pass


@rpc("reliable", "call_local", "authority")
func SendPlayerLeftLobby(ID: int) -> void:
	OnPlayerLeftLobby.emit(ID)
	pass


@rpc("reliable", "call_local", "authority")
func SendPlayerMovedToSlot(ID: int, SlotIndex: int) -> void:
	OnPlayerMovedToSlot.emit(ID, SlotIndex)
	pass


@rpc("reliable", "call_local", "authority")
func SendMapName(MapName: String) -> void:
	OnMapChanged.emit(MapName)
	pass
