extends TPowerup

class_name TGrabPowerup


func Initialize() -> void:
	Number = Constants.POWERUP_CANGRAB

func ApplyToSlot(Slot: TSlot) -> void:
	super(Slot)
	Slot.Player.PrimaryAction = Constants.PA_GRABBOMB
	pass

func ApplyOverrideToSlot(_Slot: TSlot, _Override: int) -> void:
	# TODO
	pass
