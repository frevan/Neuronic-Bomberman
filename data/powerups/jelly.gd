extends TPowerup

class_name TJellyPowerup


func Initialize() -> void:
	Number = Constants.POWERUP_JELLYBOMBS

func ApplyToSlot(Slot: TSlot) -> void:
	super(Slot)
	Slot.Player.JellyBombs = true
	pass

func ApplyOverrideToSlot(_Slot: TSlot, _Override: int) -> void:
	# TODO
	pass
