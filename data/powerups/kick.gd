extends TPowerup

class_name TKickPowerup


func Initialize() -> void:
	Number = Constants.POWERUP_CANKICK

func ApplyToSlot(Slot: TSlot) -> void:
	super(Slot)
	Slot.Player.CanKick = true
	pass

func ApplyOverrideToSlot(Slot: TSlot, Override: int) -> void:
	Slot.Player.CanKick = (Override != 0)
	pass
