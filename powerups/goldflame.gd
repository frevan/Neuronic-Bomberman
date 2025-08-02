extends TPowerup

class_name TGoldFlamePowerup


func Initialize() -> void:
	Number = Constants.POWERUP_GOLDFLAME

func ApplyToSlot(Slot: Types.TSlot) -> void:
	Slot.BombStrength = Constants.BOMB_STRENGTH_MAX
	pass

func ApplyOverrideToSlot(Slot: Types.TSlot, Override: int) -> void:
	Slot.BombStrength = min(1, Override)
	pass
