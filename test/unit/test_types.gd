extends GutTest

var _types: TTypes


func before_each():
	_types = TTypes.new()

func after_each():
	_types.queue_free()


func test_SlotIndexIsInvalid() -> void:
	var slot: TTypes.TSlot = TTypes.TSlot.new()
	assert_eq(slot.Index, Constants.INVALID_SLOT)

func test_SlotIndexIsCorrect() -> void:
	var slot: TTypes.TSlot = TTypes.TSlot.new(15)
	assert_eq(slot.Index, 15)

func test_SlotPlayerPeerIDIsZero() -> void:
	var slot: TTypes.TSlot = TTypes.TSlot.new()
	assert_eq(slot.PlayerID, 0)
