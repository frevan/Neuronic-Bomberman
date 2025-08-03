extends Node

class_name TSlot


var Index: int
var PlayerID: int
var PlayerName: String
var Ready: bool
var Position: Vector2
var TotalBombs: int
var DroppedBombs: int
var DroppingBombs: bool
var BombStrength: int
var Alive: bool
var Score: int
var Speed: int
var Diseases: Array[bool]
var Powerups: Dictionary # key=powerup number, value=TPowerup


func _init(SetIndex: int = Constants.INVALID_SLOT) -> void:
	Index = SetIndex
	PlayerID = 0
	PlayerName = ""
	Ready = false
	Position = Vector2(-1, -1)
	TotalBombs = 0
	DroppedBombs = 0
	DroppingBombs = false
	Alive = false
	Score = 0
	BombStrength = Constants.BOMB_STRENGTH
	Speed = Constants.SPEED_MIN
	Diseases.resize(Constants.NUM_DISEASES)
	for i in Constants.NUM_DISEASES:
		Diseases[i] = false
	pass
