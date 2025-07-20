extends Node

class_name TTypes


const INVALID_SLOT = -1


const FIELD_EMPTY = 0
const FIELD_SOLID = 1
const FIELD_BRICK = 2

const MAP_WIDTH = 15
const MAP_HEIGHT = 11


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
	func _init(SetName: String = "", SetColumns: int = MAP_WIDTH, SetRows: int = MAP_HEIGHT) -> void:
		Name = SetName
		for y in SetRows:
			Fields.append([])
			for x in SetColumns:
				Fields[y].append(FIELD_EMPTY)
		pass
