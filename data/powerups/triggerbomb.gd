extends TPowerup

class_name TTriggerBombPowerup


func Initialize() -> void:
	Number = Constants.POWERUP_TRIGGER

func ApplyToSlot(Slot: TSlot) -> void:
	_AddToSlot(Slot)
	pass

func ApplyOverrideToSlot(_Slot: TSlot, _Override: int) -> void:
	# TODO
	pass
