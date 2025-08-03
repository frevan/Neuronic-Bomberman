extends Node

class_name TTypes


const FIELD_EMPTY = 0
const FIELD_SOLID = 1
const FIELD_BRICK = 2
const FIELD_PU_FIRST = 100
const FIELD_PU_EXTRABOMB = 100
const FIELD_PU_MOREFLAME = 101
const FIELD_PU_DISEASE = 102
const FIELD_PU_CANKICK = 103
const FIELD_PU_EXTRASPEED = 104
const FIELD_PU_CANPUNCH = 105
const FIELD_PU_CANGRAB = 106
const FIELD_PU_SPOOGER = 107
const FIELD_PU_GOLDFLAME = 108
const FIELD_PU_TRIGGER = 109
const FIELD_PU_JELLYBOMBS = 110
const FIELD_PU_BADDISEASE = 111
const FIELD_PU_RANDOM = 112
const FIELD_PU_LAST = 112

const MAP_WIDTH = 15
const MAP_HEIGHT = 11


class TDiseases:
	var ReversControls: bool
	var Slow: bool
	var Diarrhea: bool
	var Constipated: bool
	var RandomMovement: bool
	var StickyMovement: bool

class TSlot:
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


class TBomb:
	var PlayerID: int
	var Field: Vector2i
	var TimeUntilExplosion: float # in seconds
	var Strength: int


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
	func Assign(Source: TMapPowerUp) -> void:
		Number = Source.Number
		BornWith = Source.BornWith
		HasOverride = Source.HasOverride
		OverrideValue = Source.OverrideValue
		Forbidden = Source.Forbidden


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
