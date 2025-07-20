extends Node

class_name TGameData

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


func MovePlayerToSlotIfFree(PlayerID, SlotIndex) -> bool:
	if PlayerID <= 0:
		return false
	if (SlotIndex < 0) || (SlotIndex >= Network.MAX_CLIENTS):
		return false
	
	if (Slots[SlotIndex].Player.PeerID != 0) && (Slots[SlotIndex].Player.PeerID != PlayerID):
		return false
	
	MovePlayerToSlot(PlayerID, SlotIndex)
	return true


func MovePlayerToSlot(PlayerID, SlotIndex) -> void:
	if PlayerID <= 0:
		pass
	if (SlotIndex < 0) || (SlotIndex >= Network.MAX_CLIENTS):
		pass
	
	ClearSlotForPlayer(PlayerID)
	Slots[SlotIndex].Player.PeerID = PlayerID
	pass
