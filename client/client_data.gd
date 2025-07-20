extends Node

class_name TClientData

var Slots: Array

func InitSlots() -> void:
	Slots.resize(Network.MAX_CLIENTS)
	for i in Slots.size():
		Slots[i] = Types.TSlot.new(i)
	pass


func ClearSlot(Index: int) -> void:
	Slots[Index].Player.PeerID = 0
	pass


func ClearSlotForPlayer(PlayerID: int) -> void:
	for i in Slots.size():
		if Slots[i].Player.PeerID == PlayerID:
			ClearSlot(i)
	pass


func FindSlotForPlayer(PlayerID: int) -> int:
	for i in Slots.size():
		if Slots[i].Player.PeerID == PlayerID:
			return i
	return Types.INVALID_SLOT
