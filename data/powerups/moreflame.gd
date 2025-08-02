extends TPowerup

class_name TMoreFlamePowerup


func Initialize() -> void:
	Number = Constants.POWERUP_MOREFLAME

func ApplyToSlot(Slot: Types.TSlot) -> void:
	Slot.BombStrength += 1
	pass

func ApplyOverrideToSlot(Slot: Types.TSlot, Override: int) -> void:
	Slot.BombStrength = min(1, Override)
	pass
