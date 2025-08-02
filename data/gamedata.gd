extends Node

class_name TGameData

var Slots: Array # Types.TSlot
var Map: Types.TMap = null
var Bombs: Dictionary # key: field (Vector2i), value (Types.TBomb)
var Explosions: Dictionary # key: field (Vector2i), value (Types.TExplosion)
var NumRounds: int = 1
var CurrentRound: int = -1
var AvailablePowerups: Array # Types.TMapPowerUp

var CountingDown: bool = false
var CountDownTime: float = 0


func InitSlots() -> void:
	Slots.resize(Network.MAX_CLIENTS)
	for i in Slots.size():
		Slots[i] = Types.TSlot.new(i)
	pass

func FindFreeSlotIndex() -> int:
	for i in Slots.size():
		if Slots[i].PlayerID == 0:
			return i
	return Constants.INVALID_SLOT

func ClearSlot(Index: int) -> void:
	Slots[Index].PlayerID = 0
	pass

func ClearSlotForPlayer(PlayerID: int) -> void:
	for i in Slots.size():
		if Slots[i].PlayerID == PlayerID:
			ClearSlot(i)
	pass

func FindSlotForPlayer(PlayerID: int) -> int:
	for i in Slots.size():
		if Slots[i].PlayerID == PlayerID:
			return i
	return Constants.INVALID_SLOT

func MovePlayerToSlotIfFree(PlayerID, SlotIndex) -> bool:
	if PlayerID <= 0:
		return false
	if (SlotIndex < 0) || (SlotIndex >= Network.MAX_CLIENTS):
		return false
	
	if (Slots[SlotIndex].PeerID != 0) && (Slots[SlotIndex].PlayerID != PlayerID):
		return false
	
	MovePlayerToSlot(PlayerID, SlotIndex)
	return true

func MovePlayerToSlot(PlayerID, SlotIndex) -> void:
	if PlayerID <= 0:
		pass
	if (SlotIndex < 0) || (SlotIndex >= Network.MAX_CLIENTS):
		pass
	
	ClearSlotForPlayer(PlayerID)
	Slots[SlotIndex].PlayerID = PlayerID
	pass


func AreAllPlayersReady() -> bool:
	for i in Slots.size():
		if Slots[i].PlayerID != 0:
			if !Slots[i].Ready:
				return false
	return true

func SetAllPlayersUnready() -> void:
	for i in Slots.size():
		Slots[i].Ready = false
	pass

func SetPlayerReady(PlayerID: int, Ready: bool) -> void:
	var idx = FindSlotForPlayer(PlayerID)
	if idx != Constants.INVALID_SLOT:
		Slots[idx].Ready = Ready
	pass

func SetPlayersToStartPositions() -> void:
	assert(is_instance_valid(Map))
	for i in Slots.size():
		if Slots[i].PlayerID != 0:
			if Map.StartPositions.has(i):
				Slots[i].Position = Map.StartPositions[i].Pos
	pass

func ResetPlayersBeforeMatch() -> void:
	ResetPlayersBeforeRound()
	for slot: Types.TSlot in Slots:
		slot.Score = 0
	pass

func ResetPlayersBeforeRound() -> void:
	for slot: Types.TSlot in Slots:
		slot.TotalBombs = 1
		slot.DroppedBombs = 0
		slot.DroppingBombs = false
		slot.Alive = slot.PlayerID != 0
	pass

func CountAlivePlayers() -> int:
	var count: int = 0
	for i in Slots.size():
		if Slots[i].Alive:
			count += 1
	return count

func UpdatePlayerScore(PlayerID: int, Score: int) -> void:
	var idx = FindSlotForPlayer(PlayerID)
	if idx != Constants.INVALID_SLOT:
		Slots[idx].Score = Score
	pass


func ResetBombsEtcBeforeRound() -> void:
	Bombs.clear()
	Explosions.clear()
	pass

func AddBombAt(Field: Vector2i, PlayerID: int) -> void:
	if !FieldHasBomb(Field):
		var slot_idx = FindSlotForPlayer(PlayerID)
		if slot_idx == Constants.INVALID_SLOT:
			return
		var b = Types.TBomb.new()
		b.Field = Field
		b.PlayerID = PlayerID
		b.TimeUntilExplosion = Constants.BOMB_TIME
		b.Strength = Slots[slot_idx].BombStrength
		Bombs[Field] = b
	pass

func FieldHasBomb(Field: Vector2i) -> bool:
	return Bombs.has(Field)

func RemoveBomb(Field: Vector2i) -> void:
	Bombs.erase(Field)
	pass


func FieldHasExplosion(Field: Vector2i) -> bool:
	return Explosions.has(Field)

func AddExplosionAt(Field: Vector2i) -> void:
	var explosion: Types.TExplosion
	if Explosions.has(Field):
		explosion = Explosions[Field]
	else:
		explosion = Types.TExplosion.new()
	explosion.Field = Field
	explosion.RemainingTime = Constants.EXPLOSION_TIME
	Explosions[Field] = explosion
	pass

func ResetExplosionTime(Field: Vector2i) -> void:
	if Explosions.has(Field):
		var explosion: Types.TExplosion = Explosions[Field]
		explosion.RemainingTime = Constants.EXPLOSION_TIME
	pass

func RemoveExplosion(Field: Vector2i) -> void:
	Explosions.erase(Field)
	pass

func ResetExplosionRemainingTime(Field: Vector2i) -> void:
	if Explosions.has(Field):
		var explosion: Types.TExplosion = Explosions[Field]
		explosion.RemainingTime = Constants.EXPLOSION_TIME
	pass


func SetMap(NewValue: Types.TMap) -> void:
	Map = NewValue
	InitAvailablePowerups()
	pass


func InitAvailablePowerups() -> void:
	AvailablePowerups.clear()
	for key in Map.PowerUps:
		var pu: Types.TMapPowerUp = Map.PowerUps[key]
		if !pu.Forbidden:
			var p = Types.TMapPowerUp.new()
			p.Assign(pu)
			AvailablePowerups.append(p)
	pass
