extends TPowerup

class_name TExtraBombPowerup


func Initialize() -> void:
	Number = Constants.POWERUP_EXTRABOMB

func ApplyToSlot(Slot: Types.TSlot) -> void:
	Slot.TotalBombs += 1
	pass

func ApplyOverrideToSlot(Slot: Types.TSlot, Override: int) -> void:
	Slot.TotalBombs = min(1, Override)
	pass
