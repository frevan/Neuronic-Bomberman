extends Node

class_name TSlot


var Index: int
var PlayerID: int
var PlayerName: String
var Ready: bool
var Score: int
var Player: TPlayingData

class TPlayingData:
	var Position: Vector2
	var TotalBombs: int
	var DroppedBombs: int
	var DroppingBombs: bool
	var BombStrength: int
	var Alive: bool
	var Speed: int
	var Diseases: Array[bool]
	var Powerups: Dictionary # key=powerup number, value=TPowerup
	func _init() -> void:
		Position = Vector2(-1, -1)
		TotalBombs = 0
		DroppedBombs = 0
		DroppingBombs = false
		Alive = false
		BombStrength = Constants.BOMB_STRENGTH
		Speed = Constants.SPEED_MIN
		Diseases.resize(Constants.NUM_DISEASES)
		for i in Constants.NUM_DISEASES:
			Diseases[i] = false
	func AssignWithoutPosition(Source: TPlayingData) -> void:
		TotalBombs = Source.TotalBombs
		DroppedBombs = Source.DroppedBombs
		DroppingBombs = Source.DroppingBombs
		Alive = Source.Alive
		BombStrength = Source.BombStrength
		Speed = Source.Speed
		for i in Constants.NUM_DISEASES:
			Diseases[i] = Source.Diseases[i]


func _init(SetIndex: int = Constants.INVALID_SLOT) -> void:
	Index = SetIndex
	PlayerID = 0
	PlayerName = ""
	Ready = false
	Score = 0
	Player = TPlayingData.new()
	pass
