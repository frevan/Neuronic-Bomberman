extends TPowerup

class_name TDiseasePowerup


func Initialize() -> void:
	Number = Constants.POWERUP_DISEASE
	WillClearDiseases = false

func ApplyToSlot(Slot: TSlot) -> void:
	super(Slot)
	var disease: int = randi_range(0, Constants.NUM_DISEASES - 1)
	Slot.Player.Diseases[disease] = Constants.DISEASE_TIME
	pass

func ApplyOverrideToSlot(Slot: TSlot, Override: int) -> void:
	if (Override >= 0) && (Override < Constants.NUM_DISEASES):
		Slot.Player.Diseases[Override] = Constants.DISEASE_TIME
	pass
