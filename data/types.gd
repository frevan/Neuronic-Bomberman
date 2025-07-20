extends Node

class_name TTypes

const INVALID_SLOT = -1

class TPlayer:
	var PeerID: int
	func _init() -> void:
		PeerID = 0

class TSlot:
	var Index: int
	var Player: TPlayer
	func _init(SetIndex: int = INVALID_SLOT) -> void:
		Index = SetIndex
		Player = TPlayer.new()
		pass
