extends Node

class_name TPlayingState


var Position: Vector2
var TimeBeforeNextTriggeredBomb: float # in seconds
var CanGrabBombNow: bool = false
var HasGrabbedBombID: int = Constants.INVALID_BOMB_ID

var TotalBombs: int
var NumTriggerBombs: int
var DroppedBombs: int
var HoldingPrimaryKey: bool
var HoldingSecondaryKey: bool
var BombStrength: int
var Alive: bool
var Speed: int
var Diseases: Array[float]
var PrimaryAction: int # drop bomb, spooger or grab (see PA_ constants)
var CanKick: bool
var CanPunch: bool
var Direction: int


const KEY_TOTALBOMBS = "total_bombs"
const KEY_NUMTRIGGERBOMBS = "num_trigger_bombs"
const KEY_DROPPEDBOMBS = "dropped_bombs"
const KEY_PRIMARYKEY = "primary"
const KEY_SECONDARYKEY = "secondary"
const KEY_ALIVE = "alive"
const KEY_BOMBSTRENGTH = "bomb_strength"
const KEY_SPEED = "speed"
const KEY_DISEASE = "disease_"
const KEY_PRIMARYACTION = "primary_action"
const KEY_CANKICK = "can_kick"
const KEY_CANPUNCH = "can_punch"
const KEY_DIRECTION = "direction"


func _init() -> void:
	Diseases.resize(Constants.NUM_DISEASES)
	Position = Vector2(-1, -1)
	ResetWithoutPosition()
	
func ResetWithoutPosition() -> void:
	TotalBombs = 3
	NumTriggerBombs = 0
	DroppedBombs = 0
	HoldingPrimaryKey = false
	HoldingSecondaryKey = false
	Alive = false
	BombStrength = Constants.BOMB_STRENGTH
	Speed = Constants.SPEED_MIN
	ClearDiseases()
	TimeBeforeNextTriggeredBomb = 0
	PrimaryAction = Constants.PA_NONE
	CanKick = false
	CanPunch = false
	Direction = Constants.DIRECTION_DOWN
	
func AssignWithoutPosition(Source: TPlayingState) -> void:
	TotalBombs = Source.TotalBombs
	NumTriggerBombs = Source.NumTriggerBombs
	DroppedBombs = Source.DroppedBombs
	HoldingPrimaryKey = Source.HoldingPrimaryKey
	HoldingSecondaryKey = Source.HoldingSecondaryKey
	Alive = Source.Alive
	BombStrength = Source.BombStrength
	Speed = Source.Speed
	for i in Constants.NUM_DISEASES:
		Diseases[i] = Source.Diseases[i]
	TimeBeforeNextTriggeredBomb = Source.TimeBeforeNextTriggeredBomb
	PrimaryAction = Source.PrimaryAction
	CanKick = Source.CanKick
	CanPunch = Source.CanPunch
	Direction = Source.Direction

func ToJSONString() -> String:
	var d: Dictionary
	d.set(KEY_TOTALBOMBS, TotalBombs)
	d.set(KEY_NUMTRIGGERBOMBS, NumTriggerBombs)
	d.set(KEY_DROPPEDBOMBS, DroppedBombs)
	d.set(KEY_PRIMARYKEY, HoldingPrimaryKey)
	d.set(KEY_SECONDARYKEY, HoldingSecondaryKey)
	d.set(KEY_ALIVE, Alive)
	d.set(KEY_BOMBSTRENGTH, BombStrength)
	d.set(KEY_SPEED, Speed)
	for i in Constants.NUM_DISEASES:
		d.set(KEY_DISEASE + str(i), Diseases[i])
	d.set(KEY_PRIMARYACTION, PrimaryAction)
	d.set(KEY_CANKICK, CanKick)
	d.set(KEY_CANPUNCH, CanPunch)
	d.set(KEY_DIRECTION, Direction)
	return JSON.stringify(d)

func FromJSONString(Source: String) -> void:
	ResetWithoutPosition()
	var d: Dictionary = JSON.parse_string(Source)
	if d.has(KEY_TOTALBOMBS):
		TotalBombs = d[KEY_TOTALBOMBS]
	if d.has(KEY_NUMTRIGGERBOMBS):
		NumTriggerBombs = d[KEY_NUMTRIGGERBOMBS]
	if d.has(KEY_DROPPEDBOMBS):
		DroppedBombs = d[KEY_DROPPEDBOMBS]
	if d.has(KEY_PRIMARYKEY):
		HoldingPrimaryKey = d[KEY_PRIMARYKEY]
	if d.has(KEY_SECONDARYKEY):
		HoldingSecondaryKey = d[KEY_SECONDARYKEY]
	if d.has(KEY_ALIVE):
		Alive = d[KEY_ALIVE]
	if d.has(KEY_BOMBSTRENGTH):
		BombStrength = d[KEY_BOMBSTRENGTH]
	if d.has(KEY_SPEED):
		Speed = d[KEY_SPEED]
	for i in Constants.NUM_DISEASES:
		if d.has(KEY_DISEASE + str(i)):
			Diseases[i] = d[KEY_DISEASE + str(i)]
	if d.has(KEY_PRIMARYACTION):
		PrimaryAction = d[KEY_PRIMARYACTION]
	if d.has(KEY_CANKICK):
		CanKick = d[KEY_CANKICK]
	if d.has(KEY_CANPUNCH):
		CanPunch = d[KEY_CANPUNCH]
	if d.has(KEY_DIRECTION):
		Direction = d[KEY_DIRECTION]
	pass


func ClearDiseases() -> void:
	for i in Constants.NUM_DISEASES:
		Diseases[i] = 0
	pass
