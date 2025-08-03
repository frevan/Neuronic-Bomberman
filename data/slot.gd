extends Node

class_name TSlot


var Index: int
var PlayerID: int
var PlayerName: String
var Ready: bool
var Score: int
var Player: TPlayingState


func _init(SetIndex: int = Constants.INVALID_SLOT) -> void:
	Index = SetIndex
	PlayerID = 0
	PlayerName = ""
	Ready = false
	Score = 0
	Player = TPlayingState.new()
	pass
