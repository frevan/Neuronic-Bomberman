extends Node

class_name TServerData

var Slots: Array

func InitSlots() -> void:
	Slots.resize(Network.MAX_CLIENTS)
	for i in Slots.size():
		Slots[i] = Types.TSlot.new(i)
	pass

func FindFreeSlotIndex() -> int:
	for i in Slots.size():
		if Slots[i].Player.PeerID == 0:
			return i
	return Types.INVALID_SLOT

func ClearSlot(Index: int) -> void:
	Slots[Index].Player.PeerID = 0
	pass
