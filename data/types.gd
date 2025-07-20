extends Node

class_name TTypes

class TPlayer:
	var PeerID: int
	func _init() -> void:
		PeerID = 0

class TSlot:
	var Index: int
	var Player: TPlayer
	func _init(SetIndex: int = -1) -> void:
		Index = SetIndex
		Player = TPlayer.new()
		pass
