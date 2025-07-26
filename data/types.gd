extends Node

class_name TTypes


const INVALID_SLOT = -1


const FIELD_EMPTY = 0
const FIELD_SOLID = 1
const FIELD_BRICK = 2

const MAP_WIDTH = 15
const MAP_HEIGHT = 11


const BOMB_TIME = 1.0 # in seconds 
const EXPLOSION_TIME = 1.0 # in seconds


class TSlot:
	var Index: int
	var PlayerID: int
	var Ready: bool
	var Position: Vector2
	var TotalBombs: int
	var DroppedBombs: int
	var DroppingBombs: bool
	var Alive: bool
	var Score: int
	func _init(SetIndex: int = INVALID_SLOT) -> void:
		Index = SetIndex
		PlayerID = 0
		Ready = false
		Position = Vector2(-1, -1)
		TotalBombs = 0
		DroppedBombs = 0
		DroppingBombs = false
		Alive = false
		Score = 0
		pass


class TBomb:
	var PlayerID: int
	var Field: Vector2i
	var TimeUntilExplosion: float # in seconds


class TExplosion:
	var Field: Vector2i
	var RemainingTime: float # in seconds


class TMapStartPosition:
	var Slot: int
	var Pos: Vector2i
	var Primary: bool


class TMapPowerUp:
	var Number: int
	var BornWith: bool
	var HasOverride: bool
	var OverrideValue: int
	var Forbidden: bool
	

class TMap:
	var Name: String
	var Caption: String
	var Version: int
	var BrickDensity: int
	var StartPositions: Dictionary
	var PowerUps: Dictionary
	var Fields: Array
	var Width: int
	var Height: int
	func initialize(SetName: String, SetColumns: int = MAP_WIDTH, SetRows: int = MAP_HEIGHT) -> void:
		Name = SetName
		Width = SetColumns
		Height = SetRows
		for y in Height:
			Fields.append([])
			for x in Width:
				Fields[y].append(FIELD_EMPTY)
		pass
