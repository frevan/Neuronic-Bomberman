extends TPowerup

class_name TMoreFlamePowerup


func Initialize() -> void:
	Number = Constants.POWERUP_MOREFLAME

func ApplyToSlot(Slot: TSlot) -> void:
	Slot.Player.BombStrength += 1
	pass

func ApplyOverrideToSlot(Slot: TSlot, Override: int) -> void:
	Slot.Player.BombStrength = max(1, Override)
	pass
