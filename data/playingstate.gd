extends Node

class_name TPlayingState


var Position: Vector2
var TotalBombs: int
var NumTriggerBombs: int
var DroppedBombs: int
var DroppingBombs: bool
var BombStrength: int
var Alive: bool
var Speed: int
var Diseases: Array[bool]


const KEY_TOTALBOMBS = "total_bombs"
const KEY_NUMTRIGGERBOMBS = "num_trigger_bombs"
const KEY_DROPPEDBOMBS = "dropped_bombs"
const KEY_DROPPINGBOMBS = "dropping_bombs"
const KEY_ALIVE = "alive"
const KEY_BOMBSTRENGTH = "bomb_strength"
const KEY_SPEED = "speed"
const KEY_DISEASE = "disease_"


func _init() -> void:
	Diseases.resize(Constants.NUM_DISEASES)
	Position = Vector2(-1, -1)
	ResetWithoutPosition()
	
func ResetWithoutPosition() -> void:
	TotalBombs = 1
	NumTriggerBombs = 0
	DroppedBombs = 0
	DroppingBombs = false
	Alive = false
	BombStrength = Constants.BOMB_STRENGTH
	Speed = Constants.SPEED_MIN
	for i in Constants.NUM_DISEASES:
		Diseases[i] = false
	
func AssignWithoutPosition(Source: TPlayingState) -> void:
	TotalBombs = Source.TotalBombs
	NumTriggerBombs = Source.NumTriggerBombs
	DroppedBombs = Source.DroppedBombs
	DroppingBombs = Source.DroppingBombs
	Alive = Source.Alive
	BombStrength = Source.BombStrength
	Speed = Source.Speed
	for i in Constants.NUM_DISEASES:
		Diseases[i] = Source.Diseases[i]

func ToJSONString() -> String:
	var d: Dictionary
	d.set(KEY_TOTALBOMBS, TotalBombs)
	d.set(KEY_NUMTRIGGERBOMBS, NumTriggerBombs)
	d.set(KEY_DROPPEDBOMBS, DroppedBombs)
	d.set(KEY_DROPPINGBOMBS, DroppingBombs)
	d.set(KEY_ALIVE, Alive)
	d.set(KEY_BOMBSTRENGTH, BombStrength)
	d.set(KEY_SPEED, Speed)
	for i in Constants.NUM_DISEASES:
		d.set(KEY_DISEASE + str(i), Diseases[i])
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
	if d.has(KEY_DROPPINGBOMBS):
		DroppingBombs = d[KEY_DROPPINGBOMBS]
	if d.has(KEY_ALIVE):
		Alive = d[KEY_ALIVE]
	if d.has(KEY_BOMBSTRENGTH):
		BombStrength = d[KEY_BOMBSTRENGTH]
	if d.has(KEY_SPEED):
		Speed = d[KEY_SPEED]
	for i in Constants.NUM_DISEASES:
		if d.has(KEY_DISEASE + str(i)):
			Diseases[i] = d[KEY_DISEASE + str(i)]
	pass
