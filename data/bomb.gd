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


const KEY_IsMoving = "moving"
const KEY_IsPunched = "punched"
#const KEY_PunchDirection = "punch_direction"
#const KEY_PunchStartField = "punch_start"
#const KEY_PunchEndField = "punch_end"


func ToJSONString() -> String:
	var d: Dictionary
	d.set(KEY_IsMoving, IsMoving)
	d.set(KEY_IsPunched, IsPunched)
	#d.set(KEY_PunchDirection, PunchDirection)
	#d.set(KEY_PunchStartField, PunchStartField)
	#d.set(KEY_PunchEndField, PunchEndField)
	return JSON.stringify(d)

func FromJSONString(Source: String) -> void:
	var d: Dictionary = JSON.parse_string(Source)
	if d.has(KEY_IsMoving):
		IsMoving = d[KEY_IsMoving]
	if d.has(KEY_IsPunched):
		IsPunched = d[KEY_IsPunched]
	#if d.has(KEY_PunchDirection):
		#PunchDirection = d[KEY_PunchDirection]
	#if d.has(KEY_PunchStartField):
		#PunchStartField = d[KEY_PunchStartField]
	#if d.has(KEY_PunchEndField):
		#PunchEndField = d[KEY_PunchEndField]
	pass
