extends GutTest

var o: TGameData


func before_each():
	o = TGameData.new()
	o.InitSlots()

func after_each():
	o.queue_free()

func LoadTestMap() -> void:
	var Maps: TMaps = TMaps.new()
	o.Slots[1].PlayerID = 5
	o.Map = Maps.LoadMap("test.sch")


func test_SlotsAreInitialized() -> void:
	assert_ne(o.Slots.size(), 0)

func test_SlotsAreNotNull() -> void:
	for i in o.Slots.size():
		assert_not_null(o.Slots[i])


func test_FindFreeSlotIndexWhenNotFull() -> void:
	o.Slots[0].PlayerID = 1 # one occupied slot
	var index: int = o.FindFreeSlotIndex()
	assert_ne(index, Constants.INVALID_SLOT)

func test_FindFreeSlotIndexWhenFull() -> void:
	for i in o.Slots.size():
		o.Slots[i].PlayerID = 1
	var index: int = o.FindFreeSlotIndex()
	assert_eq(index, Constants.INVALID_SLOT)



func test_ClearSlot() -> void:
	o.Slots[0].PlayerID = 1
	o.ClearSlot(0)
	assert_eq(o.Slots[0].PlayerID, 0)

func test_ClearSlotForPlayer() -> void:
	o.Slots[1].PlayerID = 1
	o.ClearSlotForPlayer(1)
	assert_eq(o.Slots[1].PlayerID, 0)

func test_DontClearSlotForOtherPlayers() -> void:
	o.Slots[0].PlayerID = 1
	o.Slots[1].PlayerID = 2
	o.ClearSlotForPlayer(2)
	assert_eq(o.Slots[0].PlayerID, 1)


func test_FindSlotForUnknownPlayer() -> void:
	var idx = o.FindSlotForPlayer(123)
	assert_eq(idx, Constants.INVALID_SLOT)

func test_FindSlotForKnownPlayer() -> void:
	o.Slots[1].PlayerID = 2
	var idx = o.FindSlotForPlayer(2)
	assert_eq(idx, 1)


func test_AreAllPlayersReady() -> void:
	for i in o.Slots.size():
		if i % 2 == 0:
			o.Slots[i].PlayerID = 123
			o.Slots[i].Ready = true
	assert_true(o.AreAllPlayersReady())

func test_SetAllPlayersUnready() -> void:
	o.Slots[1].PlayerID = 123
	o.Slots[1].Ready = false
	o.Slots[2].PlayerID = 456
	o.Slots[2].Ready = true
	o.SetAllPlayersUnready()
	assert_false(o.Slots[1].Ready)
	assert_false(o.Slots[2].Ready)
	
func test_SetPlayerReady() -> void:
	o.Slots[1].PlayerID = 123
	o.Slots[1].Ready = false
	o.Slots[2].PlayerID = 456
	o.Slots[2].Ready = true
	o.SetPlayerReady(123, true)
	o.SetPlayerReady(456, false)
	assert_true(o.Slots[1].Ready)
	assert_false(o.Slots[2].Ready)


func test_SetPlayersToStartPositions() -> void:
	LoadTestMap()
	assert_not_null(o.Map)
	o.Slots[1].PlayerID = 5
	o.SetPlayersToStartPositions()
	assert_almost_eq(o.Slots[1].Position.x, 14, 0.1)
	assert_almost_eq(o.Slots[1].Position.y, 10, 0.1)

func test_ResetPlayersBeforeMatch() -> void:
	o.Slots[1].Score = 123
	o.ResetPlayersBeforeMatch()
	assert_eq(o.Slots[1].Score, 0)
	pass

func test_ResetPlayersBeforeRound() -> void:
	o.Slots[1].PlayerID = 123
	o.Slots[1].TotalBombs = 5
	o.Slots[1].DroppedBombs = 4
	o.Slots[1].DroppingBombs = true
	o.Slots[1].Alive = false
	o.ResetPlayersBeforeRound()
	assert_eq(o.Slots[1].TotalBombs, 1)
	assert_eq(o.Slots[1].DroppedBombs, 0)
	assert_false(o.Slots[1].DroppingBombs)
	assert_true(o.Slots[1].Alive)
	pass
