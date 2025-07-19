extends Node


const PORT = 15063
const MAX_CLIENTS = 10
const CONNECTION_TIMEOUT = 3000

var Peer: ENetMultiplayerPeer
var PeerID: int


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
