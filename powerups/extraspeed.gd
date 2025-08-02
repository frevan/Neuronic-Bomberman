extends TPowerup

class_name TExtraSpeedPowerup


func Initialize() -> void:
	Number = Constants.POWERUP_EXTRASPEED

func ApplyToSlot(Slot: Types.TSlot) -> void:
	Slot.Speed += 1
	pass

func ApplyOverrideToSlot(Slot: Types.TSlot, Override: int) -> void:
	Slot.Speed = min(1, Types.SPEED_MIN + Override)
	pass
