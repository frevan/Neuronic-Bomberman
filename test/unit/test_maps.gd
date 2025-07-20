extends GutTest

var TMaps = preload("res://data/maps.gd")
var o: TMaps
var m: Types.TMap

func before_each():
	o = TMaps.new()
	m = Types.TMap.new()


func after_each():
	o.queue_free()
	m = null


func test_IsValidFieldPos_Valid() -> void:
	var pos: Vector2i = Vector2i(3, 3)
	assert_true(o._IsValidFieldPos(m, pos))


func test_IsValidFieldPos_Invalid() -> void:
	var pos: Vector2i = Vector2i(-1, 500)
	assert_false(o._IsValidFieldPos(m, pos))


func test_SetFieldTypeTo() -> void:
	var pos: Vector2i = Vector2i(5, 5)
	o.SetFieldTypeTo(m, pos, Types.FIELD_SOLID)
	assert_eq(m.Fields[pos.y][pos.x], Types.FIELD_SOLID)


func test_GetFieldType() -> void:
	var pos: Vector2i = Vector2i(5, 5)
	m.Fields[pos.y][pos.x] = Types.FIELD_BRICK
	var v: int = o.GetFieldType(m, pos)
	assert_eq(v, Types.FIELD_BRICK)


func test_ClearField() -> void:
	var pos: Vector2i = Vector2i(5, 5)
	o.ClearField(m, pos)
	assert_eq(o.GetFieldType(m, pos), Types.FIELD_EMPTY)


func test_ClearFieldsAround() -> void:
	for y in m.Height:
		for x in m.Width:
			o.SetFieldTypeTo(m, Vector2i(x, y), Types.FIELD_BRICK)
	
	var pos: Vector2i = Vector2i(5, 5)
	o.ClearFieldsAround(m, pos)
	
	assert_eq(o.GetFieldType(m, pos), Types.FIELD_EMPTY)
	assert_eq(o.GetFieldType(m, Vector2i(pos.x - 1, pos.y)), Types.FIELD_EMPTY)
	assert_eq(o.GetFieldType(m, Vector2i(pos.x + 1, pos.y)), Types.FIELD_EMPTY)
	assert_eq(o.GetFieldType(m, Vector2i(pos.x, pos.y - 1)), Types.FIELD_EMPTY)
	assert_eq(o.GetFieldType(m, Vector2i(pos.x, pos.y + 1)), Types.FIELD_EMPTY)
