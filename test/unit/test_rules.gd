extends GutTest

var o: TRules
var Data: TGameData
var Maps: TMaps


func before_each():
	o = TRules.new()
	Data = TGameData.new()
	Maps = TMaps.new()

func after_each():
	o.queue_free()
	Data.queue_free()
	Maps.queue_free()


func LoadTestMap() -> Types.TMap:
	return Maps.LoadMap("test.sch")


func test_FieldHasBrickOrSolid() -> void:
	Data.Map = LoadTestMap()
	Maps.ClearField(Data.Map, Vector2i(0, 1))
	assert_true(o._FieldHasBrickOrSolid(Data.Map, Vector2i(0, 0)))
	assert_false(o._FieldHasBrickOrSolid(Data.Map, Vector2i(0, 1)))


func test_FieldHasObstacle() -> void:
	Data.InitSlots()
	Data.Map = LoadTestMap()
	Data.Slots[0].PlayerID = 123
	Maps.ClearField(Data.Map, Vector2i(0, 1))
	Maps.ClearField(Data.Map, Vector2i(0, 2))
	Data.AddBombAt(123, Constants.INVALID_BOMB_ID, Constants.BOMB_NORMAL, Vector2(0, 2))
	assert_true(o._FieldHasObstacle(Data, Vector2i(0, 0))) # check for brick
	assert_false(o._FieldHasObstacle(Data, Vector2i(0, 1))) # check for empty field
	assert_true(o._FieldHasObstacle(Data, Vector2i(0, 2))) # check for bomb

func test_PlayerTouchesField() -> void:
	var field = Vector2i(5, 6)
	var pos = Tools.FieldPositionToScreen(Vector2(5.9, 6.3))
	assert_true(o._PlayerTouchesField(pos, field))
	pos = Tools.FieldPositionToScreen(Vector2(7.1, 15.5))
	assert_false(o._PlayerTouchesField(pos, field))

func test_LimitPlayerToArena() -> void:
	var p = o._LimitPlayerToArena(Vector2(-1, 10000))
	assert_eq(p.x, Tools.FIELD_OFFSET.x)
	assert_eq(p.y, Tools.FIELD_OFFSET.y + (Tools.FIELD_SIZE.y * (Types.MAP_HEIGHT - 1)))
