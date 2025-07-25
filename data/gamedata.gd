extends Node

class_name TGameData

var Slots: Array
var Map: Types.TMap = null
var Bombs: Dictionary # key: field (Vector2i), value: Types.TBomb


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


func AreAllPlayersReady() -> bool:
	for i in Slots.size():
		if Slots[i].Player.PeerID != 0:
			if !Slots[i].Player.Ready:
				return false
	return true


func SetAllPlayersUnready() -> void:
	for i in Slots.size():
		if is_instance_valid(Slots[i].Player):
			Slots[i].Player.Ready = false
	pass


func SetPlayerReady(PlayerID: int, Ready: bool) -> void:
	var idx = FindSlotForPlayer(PlayerID)
	if idx != Types.INVALID_SLOT:
		Slots[idx].Player.Ready = Ready
	pass


func SetPlayersToStartPositions() -> void:
	assert(is_instance_valid(Map))
	for i in Slots.size():
		var p: Types.TPlayer = Slots[i].Player
		if p.PeerID != 0:
			if Map.StartPositions.has(i):
				p.Position = Map.StartPositions[i].Pos
	pass


func ResetPlayersBeforeRound() -> void:
	for i in Slots.size():
		var p: Types.TPlayer = Slots[i].Player
		p.TotalBombs = 1
		p.DroppedBombs = 0
		p.DroppingBombs = false
	pass


func ResetBombs() -> void:
	Bombs.clear()
	pass


func AddBombAt(Field: Vector2i, PlayerID: int) -> void:
	if !FieldHasBomb(Field):
		var b = Types.TBomb.new()
		b.Field = Field
		b.PlayerID = PlayerID
		b.TimeUntilExplosion = Types.BOMB_EXPLOSIONTIME
		Bombs[Field] = b
	pass

func FieldHasBomb(Field: Vector2i) -> bool:
	return Bombs.has(Field)

func RemoveBomb(Field: Vector2i) -> void:
	Bombs.erase(Field)
	pass


func CreateExplosion(_Field: Vector2i) -> void:
	# TODO
	pass
