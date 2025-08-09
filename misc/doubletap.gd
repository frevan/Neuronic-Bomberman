extends Node

class_name TDoubleTap


const STATE_NONE = 0
const STATE_KEYUP = 1
const STATE_KEYDOWN = 2
const STATE_DOUBLETAP = 3

const MAX_TIME = 0.25


enum TDetectionStage {IDLE, FIRST, SECOND}


var Stage: TDetectionStage = TDetectionStage.IDLE
var TimeSinceKeyDown: float


func ProcessKeyDown() -> void:
	match Stage:
		TDetectionStage.IDLE:
			Stage = TDetectionStage.FIRST
			TimeSinceKeyDown = 0
		TDetectionStage.FIRST:
			if TimeSinceKeyDown <= MAX_TIME:
				Stage = TDetectionStage.SECOND
				TimeSinceKeyDown = 0
			else:
				Stage = TDetectionStage.IDLE
	pass

func Process(Delta: float) -> void:
	if Stage != TDetectionStage.IDLE:
		TimeSinceKeyDown += Delta
	pass

func ProcessKeyUp() -> bool:
	match Stage:
		TDetectionStage.FIRST:
			if TimeSinceKeyDown > MAX_TIME:
				Stage = TDetectionStage.IDLE
			TimeSinceKeyDown = 0
		TDetectionStage.SECOND:
			if TimeSinceKeyDown <= MAX_TIME:
				Stage = TDetectionStage.IDLE
				return true
	return false
