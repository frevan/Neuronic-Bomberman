extends TPowerup

class_name TRandomPowerup


func Initialize() -> void:
	Number = Constants.POWERUP_RANDOM
	WillClearDiseases = false

func ApplyToSlot(Slot: TSlot) -> void:
	super(Slot)
	
	var which: int = randi_range(0, Constants.NUM_POWERUPS - 1)
	if which == Constants.POWERUP_RANDOM:
		which = Constants.POWERUP_EXTRABOMB
	
	var powerup = Tools.CreatePowerup(which)
	if !powerup:
		powerup = Tools.CreatePowerup(Constants.POWERUP_EXTRABOMB)
	
	powerup.ApplyToSlot(Slot)
	pass

func ApplyOverrideToSlot(_Slot: TSlot, _Override: int) -> void:
	pass
