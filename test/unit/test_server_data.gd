extends GutTest

var TServerData = preload("res://server/server_data.gd")
var o: TServerData


func before_each():
	o = TServerData.new()
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
	assert_ne(index, -1)

func test_FindFreeSlotIndexWhenFull() -> void:
	for i in o.Slots.size():
		o.Slots[i].Player.PeerID = 1
	var index: int = o.FindFreeSlotIndex()
	assert_eq(index, -1)

func test_ClearSlot() -> void:
	o.Slots[0].Player.PeerID = 1
	o.ClearSlot(0)
	assert_eq(o.Slots[0].Player.PeerID, 0)
