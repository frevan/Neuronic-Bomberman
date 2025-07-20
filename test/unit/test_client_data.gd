extends GutTest

var TClientData = preload("res://client/client_data.gd")
var o: TClientData


func before_each():
	o = TClientData.new()
	o.InitSlots()

func after_each():
	o.queue_free()


func test_SlotsAreInitialized() -> void:
	assert_ne(o.Slots.size(), 0)

func test_SlotsAreNotNull() -> void:
	for i in o.Slots.size():
		assert_not_null(o.Slots[i])


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
