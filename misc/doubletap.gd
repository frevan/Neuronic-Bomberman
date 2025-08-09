extends Node

class_name TDoubleTap


const STATE_NONE = 0
const STATE_KEYUP = 1
const STATE_KEYDOWN = 2
const STATE_DOUBLETAP = 3

const MAX_TIME = 0.06


enum TDetectionStage {IDLE, FIRST, SECOND}


var Stage: TDetectionStage = TDetectionStage.IDLE
var TimeSinceKeyDown: float


func Process(Delta: float, ActionName: String) -> int:
	if Input.is_action_just_pressed(ActionName):
		_StartDetection()
	elif Input.is_action_pressed(ActionName) && (Stage != TDetectionStage.IDLE):
		return _CheckIfTimeout(Delta)
	elif Input.is_action_just_released(ActionName):
		return _ProcessKeyUp()
	return STATE_NONE


func _StartDetection():
	assert(Stage != TDetectionStage.SECOND)
	match Stage:
		TDetectionStage.IDLE:
			Stage = TDetectionStage.FIRST
			TimeSinceKeyDown = 0
		TDetectionStage.FIRST:
			Stage = TDetectionStage.SECOND
			TimeSinceKeyDown = 0
	pass

func _CheckIfTimeout(Delta: float) -> int:
	if Stage != TDetectionStage.IDLE:
		TimeSinceKeyDown += Delta
		if TimeSinceKeyDown > MAX_TIME:
			Stage = TDetectionStage.IDLE
			return STATE_KEYDOWN
	return STATE_NONE

func _ProcessKeyUp() -> int:
	match Stage:
		TDetectionStage.IDLE:
			return STATE_KEYUP
		TDetectionStage.FIRST:
			if TimeSinceKeyDown > MAX_TIME:
				return STATE_KEYUP
		TDetectionStage.SECOND:
			if TimeSinceKeyDown <= MAX_TIME:
				Stage = TDetectionStage.IDLE
				return STATE_DOUBLETAP
			else:
				return STATE_KEYUP
	return STATE_NONE
