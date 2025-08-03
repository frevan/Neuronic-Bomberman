extends TPowerup

class_name TGoldFlamePowerup


func Initialize() -> void:
	Number = Constants.POWERUP_GOLDFLAME

func ApplyToSlot(Slot: TSlot) -> void:
	Slot.BombStrength = Constants.BOMB_STRENGTH_MAX
	pass

func ApplyOverrideToSlot(Slot: TSlot, Override: int) -> void:
	Slot.BombStrength = max(1, Override)
	pass
