extends Node

class_name TBomb

var ID: int
var Type: int # see BOMB_NORMAL etc
var PlayerID: int
var Position: Vector2
var TimeUntilExplosion: float # in seconds
var TimeSinceDrop: float # in seconds
var Strength: int

var IsMoving: bool

var IsPunched: bool
var PunchDirection: Vector2i
var PunchStartField: Vector2i
var PunchEndField: Vector2i

var IsGrabbedBy: int # player id


const KEY_IsMoving = "moving"
const KEY_IsPunched = "punched"
const KEY_IsGrabbedBy = "grabbed_by"


func ToJSONString() -> String:
	var d: Dictionary
	d.set(KEY_IsMoving, IsMoving)
	d.set(KEY_IsPunched, IsPunched)
	d.set(KEY_IsGrabbedBy, IsGrabbedBy)
	return JSON.stringify(d)

func FromJSONString(Source: String) -> void:
	var d: Dictionary = JSON.parse_string(Source)
	if d.has(KEY_IsMoving):
		IsMoving = d[KEY_IsMoving]
	if d.has(KEY_IsPunched):
		IsPunched = d[KEY_IsPunched]
	if d.has(KEY_IsGrabbedBy):
		IsGrabbedBy = d[KEY_IsGrabbedBy]
	pass
