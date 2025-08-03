extends TPowerup

class_name TBadDiseasePowerup


func Initialize() -> void:
	Number = Constants.POWERUP_BADDISEASE

func ApplyToSlot(Slot: TSlot) -> void:
	for i in [0, 1, 2]:
		var disease = randi_range(0, Constants.NUM_DISEASES - 1)
		Slot.Player.Diseases[disease] = true
	pass

func ApplyOverrideToSlot(_Slot: TSlot, _Override: int) -> void:
	#if (Override >= 0) && (Override < Constants.NUM_DISEASES):
		#Slot.Player.Diseases[Override] = true
	pass
