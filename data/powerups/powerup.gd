extends Node

class_name TPowerup


var CanProcess: bool = false
var Number: int = -1
var WillClearDiseases: bool = true


func Initialize() -> void:
	assert(false) # this should be implemented in descendants
	pass

func ApplyToSlot(Slot: TSlot) -> void:
	if WillClearDiseases:
		Slot.Player.ClearDiseases()
	pass

func ApplyOverrideToSlot(_Slot: TSlot, _Override: int) -> void:
	assert(false) # this should be implemented in descendants
	pass

func Process(_Delta: float) -> void:
	assert(CanProcess)
	pass
