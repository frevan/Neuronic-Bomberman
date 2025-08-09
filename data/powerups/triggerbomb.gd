extends TPowerup

class_name TTriggerBombPowerup


func Initialize() -> void:
	Number = Constants.POWERUP_TRIGGER

func ApplyToSlot(Slot: TSlot) -> void:
	Slot.Player.NumTriggerBombs = Slot.Player.TotalBombs
	pass

func ApplyOverrideToSlot(_Slot: TSlot, _Override: int) -> void:
	# TODO
	pass
