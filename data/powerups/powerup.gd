extends Node

class_name TPowerup


var CanProcess: bool = false
var Number: int = -1


func Initialize() -> void:
	assert(false) # this should be implemented in descendants
	pass

func ApplyToSlot(_Slot: TSlot) -> void:
	assert(false) # this should be implemented in descendants
	pass

func ApplyOverrideToSlot(_Slot: TSlot, _Override: int) -> void:
	assert(false) # this should be implemented in descendants
	pass

func Process(_Delta: float) -> void:
	assert(CanProcess)
	pass


func _AddToSlot(Slot: TSlot) -> void:
	Slot.Powerups.set(Number, self)
	pass

func _RemoveFromSlot(Slot: TSlot) -> void:
	if Slot.Powerups.has(Number):
		Slot.Powerups.erase(Number)
	pass
