extends TPowerup

class_name TDiseasePowerup


func Initialize() -> void:
	Number = Constants.POWERUP_DISEASE

func ApplyToSlot(Slot: Types.TSlot) -> void:
	var disease: int = randi_range(0, Constants.NUM_DISEASES)
	Slot.Diseases[disease] = true
	pass

func ApplyOverrideToSlot(Slot: Types.TSlot, Override: int) -> void:
	if (Override >= 0) && (Override < Constants.NUM_DISEASES):
		Slot.Diseases[Override] = true
	pass
