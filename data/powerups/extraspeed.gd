extends TPowerup

class_name TExtraSpeedPowerup


func Initialize() -> void:
	Number = Constants.POWERUP_EXTRASPEED

func ApplyToSlot(Slot: TSlot) -> void:
	Slot.Player.Speed += 1
	pass

func ApplyOverrideToSlot(Slot: TSlot, Override: int) -> void:
	Slot.Player.Speed = min(1, Constants.SPEED_MIN + Override)
	pass
