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

var IsKicked: bool
var KickDirection: Vector2i
var KickStartField: Vector2i
var KickEndField: Vector2i
