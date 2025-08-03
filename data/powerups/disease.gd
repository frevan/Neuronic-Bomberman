extends TPowerup

class_name TDiseasePowerup


func Initialize() -> void:
	Number = Constants.POWERUP_DISEASE

func ApplyToSlot(Slot: TSlot) -> void:
	var disease: int = randi_range(0, Constants.NUM_DISEASES - 1)
	Slot.Player.Diseases[disease] = true
	pass

func ApplyOverrideToSlot(Slot: TSlot, Override: int) -> void:
	if (Override >= 0) && (Override < Constants.NUM_DISEASES):
		Slot.Player.Diseases[Override] = true
	pass
