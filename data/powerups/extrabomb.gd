extends TPowerup

class_name TExtraBombPowerup


func Initialize() -> void:
	Number = Constants.POWERUP_EXTRABOMB

func ApplyToSlot(Slot: TSlot) -> void:
	Slot.Player.TotalBombs = min(Slot.Player.TotalBombs + 1, Constants.MAX_BOMBS)
	pass

func ApplyOverrideToSlot(Slot: TSlot, Override: int) -> void:
	Slot.Player.TotalBombs = min(1, Override)
	pass
