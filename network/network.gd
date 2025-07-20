extends Node


# received by server only
signal OnRequestJoinLobby # params: sender_id (int)
# received by clients
signal OnResponseToJoinLobby # params: accepted (bool)


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


@rpc("reliable", "call_local", "any_peer")
func SendJoinLobby() -> void:
	var id = multiplayer.get_remote_sender_id()
	OnRequestJoinLobby.emit(id)
	pass


@rpc("reliable", "call_local", "authority")
func SendJoinLobbyResponse(Accepted: bool) -> void:
	OnResponseToJoinLobby.emit(Accepted)
	pass
