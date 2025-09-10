extends Node

class_name TGameData

var Slots: Array # TSlot
var Map: Types.TMap = null
var Bombs: Dictionary # key: bomb id (int), value: bomb (TBomb)
var Explosions: Dictionary # key: field (Vector2i), value (Types.TExplosion)
var WinCondition: Constants.WinCondition = Constants.WinCondition.NUM_ROUNDS
var NumRounds: int = 3
var ScoreToWin: int = 3
var CurrentRound: int = -1
var AvailablePowerups: Array # Types.TMapPowerUp
var MaxTime: int = 3*60 # in seconds, default to 3 minutes

var CountingDown: bool = false
var CountDownTime: float = 0

var CurrentRoundTime: float # in seconds


func InitSlots() -> void:
	Slots.resize(Network.MAX_CLIENTS)
	for i in Slots.size():
		Slots[i] = TSlot.new(i)
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

func GetSlotForPlayer(PlayerID: int) -> TSlot:
	for i in Slots.size():
		if Slots[i].PlayerID == PlayerID:
			return Slots[i]
	return null

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
				Slots[i].Player.Position = Map.StartPositions[i].Pos
	pass

func ResetPlayersBeforeMatch() -> void:
	ResetPlayersBeforeRound()
	for slot: TSlot in Slots:
		slot.Score = 0
	pass

func ResetPlayersBeforeRound() -> void:
	for slot: TSlot in Slots:
		slot.Player.ResetWithoutPosition()
		slot.Player.Alive = slot.PlayerID != 0
	pass

func CountAlivePlayers() -> int:
	var count: int = 0
	for i in Slots.size():
		if Slots[i].Player.Alive:
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

func AddBombAt(PlayerID: int, BombID: int, Type: int, Position: Vector2) -> bool:
	if GetBombInField(Position):
		return false
	var idx = FindSlotForPlayer(PlayerID)
	if idx == Constants.INVALID_SLOT:
		return false
	var b = TBomb.new()
	if BombID == Constants.INVALID_BOMB_ID:
		b.ID = Tools.NewBombID()
	else:
		b.ID = BombID
	b.Type = Type
	b.Position = Position
	b.PlayerID = PlayerID
	b.TimeUntilExplosion = Constants.BOMB_TIME
	b.Strength = Slots[idx].Player.BombStrength
	b.IsJelly = Slots[idx].Player.JellyBombs
	b.IsMoving = false
	Bombs[b.ID] = b
	if b.Type == Constants.BOMB_TRIGGER:
		var slot: TSlot = Slots[idx]
		slot.Player.NumTriggerBombs -= 1
	return true

func GetBombInField(Field: Vector2i) -> TBomb:
	for id in Bombs:
		var f: Vector2i = Bombs[id].Position
		if f == Field:
			return Bombs[id]
	return null

func GetBombForID(BombID: int) -> TBomb:
	if Bombs.has(BombID):
		return Bombs[BombID]
	return null

func RemoveBomb(BombID: int) -> void:
	Bombs.erase(BombID)
	pass

func RemoveBombAt(Field: Vector2i) -> void:
	var bomb = GetBombInField(Field)
	if bomb:
		RemoveBomb(bomb.ID)
	pass

func FindOldestTriggerBombFor(PlayerID: int) -> TBomb:
	var oldestBomb: TBomb = null
	for id in Bombs:
		var bomb: TBomb = Bombs[id]
		if (bomb.PlayerID != PlayerID) || (bomb.Type != Constants.BOMB_TRIGGER):
			continue
		if !oldestBomb:
			oldestBomb = bomb
		elif oldestBomb.TimeSinceDrop < bomb.TimeSinceDrop:
			oldestBomb = bomb
	return oldestBomb


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


func FindBombGrabbedBy(PlayerID: int) -> int:
	var bomb: TBomb = GetBombGrabbedBy(PlayerID)
	if bomb:
		return bomb.ID
	return Constants.INVALID_BOMB_ID

func GetBombGrabbedBy(PlayerID: int) -> TBomb:
	for id in Bombs:
		var bomb: TBomb = Bombs[id]
		if bomb.IsGrabbedBy == PlayerID:
			return bomb
	return null
