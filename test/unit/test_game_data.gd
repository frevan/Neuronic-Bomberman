extends GutTest

var o: TGameData


func before_each():
	o = TGameData.new()
	o.InitSlots()

func after_each():
	o.queue_free()


func test_SlotsAreInitialized() -> void:
	assert_ne(o.Slots.size(), 0)

func test_SlotsAreNotNull() -> void:
	for i in o.Slots.size():
		assert_not_null(o.Slots[i])


func test_FindFreeSlotIndexWhenNotFull() -> void:
	o.Slots[0].Player.PeerID = 1 # one occupied slot
	var index: int = o.FindFreeSlotIndex()
	assert_ne(index, Types.INVALID_SLOT)

func test_FindFreeSlotIndexWhenFull() -> void:
	for i in o.Slots.size():
		o.Slots[i].Player.PeerID = 1
	var index: int = o.FindFreeSlotIndex()
	assert_eq(index, Types.INVALID_SLOT)



func test_ClearSlot() -> void:
	o.Slots[0].Player.PeerID = 1
	o.ClearSlot(0)
	assert_eq(o.Slots[0].Player.PeerID, 0)

func test_ClearSlotForPlayer() -> void:
	o.Slots[1].Player.PeerID = 1
	o.ClearSlotForPlayer(1)
	assert_eq(o.Slots[1].Player.PeerID, 0)

func test_DontClearSlotForOtherPlayers() -> void:
	o.Slots[0].Player.PeerID = 1
	o.Slots[1].Player.PeerID = 2
	o.ClearSlotForPlayer(2)
	assert_eq(o.Slots[0].Player.PeerID, 1)


func test_FindSlotForUnknownPlayer() -> void:
	var idx = o.FindSlotForPlayer(123)
	assert_eq(idx, Types.INVALID_SLOT)

func test_FindSlotForKnownPlayer() -> void:
	o.Slots[1].Player.PeerID = 2
	var idx = o.FindSlotForPlayer(2)
	assert_eq(idx, 1)
