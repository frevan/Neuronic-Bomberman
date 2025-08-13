extends Node

class_name TRules


func ApplyPowerupToPlayer(Data: TGameData, SlotIndex: int, Powerup: int) -> void:
	assert(Data)
	assert(SlotIndex != Constants.INVALID_SLOT)
	assert(SlotIndex < Data.Slots.size())
	var slot: TSlot = Data.Slots[SlotIndex]
	var p: TPowerup = Tools.CreatePowerup(Powerup)
	if p:
		p.ApplyToSlot(slot)
	pass

func OverridePlayerPowerup(Data: TGameData, SlotIndex: int, Powerup: int, Override: int) -> void:
	assert(Data)
	assert(SlotIndex != Constants.INVALID_SLOT)
	assert(SlotIndex < Data.Slots.size())
	var slot: TSlot = Data.Slots[SlotIndex]
	var p: TPowerup = Tools.CreatePowerup(Powerup)
	if p:
		p.ApplyOverrideToSlot(slot, Override)
	pass

func ApplyInitialPowerupsToPlayers(Data: TGameData) -> void:
	var _initial_powerups: Array
	for key in Data.Map.PowerUps:
		var pu: Types.TMapPowerUp = Data.Map.PowerUps[key]
		if pu.Number == Types.FIELD_PU_RANDOM - Types.FIELD_PU_FIRST:
			continue
		if pu.BornWith || pu.HasOverride:
			_initial_powerups.append(pu)
	
	for i in Data.Slots.size():
		for powerup: Types.TMapPowerUp in _initial_powerups:
			if powerup.BornWith:
				ApplyPowerupToPlayer(Data, i, powerup.Number)
			elif powerup.HasOverride:
				OverridePlayerPowerup(Data, i, powerup.Number, powerup.OverrideValue)
	pass


func ProcessDisease_ReverseControls(_Delta: float, Direction: Vector2) -> Vector2:
	if Direction.x != 0:
		Direction.x = -Direction.x
	if Direction.y != 0:
		Direction.y = -Direction.y
	return Direction

var _Random_LastDirection: Vector2
var _Random_TimeSinceChange: float = 0

func ProcessDisease_RandomMovement(Delta: float, Direction: Vector2) -> Vector2:
	if Direction.is_zero_approx():
		return Direction
	_Random_TimeSinceChange -= Delta
	if _Random_TimeSinceChange <= 0:
		_Random_LastDirection = Vector2(randi_range(-1, 1), randi_range(-1, 1))
		_Random_TimeSinceChange = 0.1
	return _Random_LastDirection

func ProcessDisease_Slow(_Delta: float, _Speed: float) -> float:
	return Constants.SPEED_SLOW

var _Sticky_Active: bool = false
var _Sticky_TimeSinceChange: float = 0.0

func ProcessDisease_StickyMovement(Delta: float, Speed: float) -> float:
	_Sticky_TimeSinceChange -= Delta
	if _Sticky_TimeSinceChange < 0:
		_Sticky_Active = !_Sticky_Active
		_Sticky_TimeSinceChange = 0.6
	if _Sticky_Active:
		return Constants.SPEED_SLOW
	else:
		return Speed
