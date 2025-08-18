extends TPowerup

class_name TPunchPowerup


func Initialize() -> void:
	Number = Constants.POWERUP_CANPUNCH

func ApplyToSlot(Slot: TSlot) -> void:
	super(Slot)
	Slot.Player.CanPunch = true
	pass

func ApplyOverrideToSlot(Slot: TSlot, Override: int) -> void:
	Slot.Player.CanPunch = (Override != 0)
	pass
