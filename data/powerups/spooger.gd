extends TPowerup

class_name TSpoogerPowerup


func Initialize() -> void:
	Number = Constants.POWERUP_EXTRABOMB

func ApplyToSlot(Slot: TSlot) -> void:
	super(Slot)
	Slot.Player.HasSpooger = true
	pass

func ApplyOverrideToSlot(_Slot: TSlot, _Override: int) -> void:
	# TODO
	pass
