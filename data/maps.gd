extends Node

class_name TMaps


const MapsPathConst = "res://assets/maps"

var MapNames = []


func FindMapFiles() -> void:
	MapNames.clear()
	
	var dir = DirAccess.open(MapsPathConst)
	if !dir:
		print("ERROR: maps folder not found")
		return
		
	dir.list_dir_begin()
	var fname = dir.get_next()
	while fname != "":
		MapNames.append(fname)
		fname = dir.get_next()
	pass


func _ReadMapFileContents(MapName: String):
	var fname = MapsPathConst + "/" + MapName
	if !FileAccess.file_exists(fname):
		return ""
		
	var file = FileAccess.open(fname, FileAccess.READ)
	if !file:
		return ""
		
	var result = file.get_as_text()
	
	file.close()
	return result


func _ReadCommandFromSchemeLine(Line: String) -> String:
	var pos: int = Line.find(",")
	if pos > -1:
		return Line.substr(0, pos).to_lower()
	return ""


func _ReadParamsFromSchemeLine(Line: String):
	var pos: int = Line.find(",")
	if pos > -1:
		var s = Line.substr(pos + 1)
		return s.split(",")
	return []


func _ProcessMapVersion(Params: Array, Map: Types.TMap) -> bool:
	if Params.size() != 1:
		return false
	Map.Version = Params[0].to_int()
	return Map.Version == 2


func _ProcessMapName(Params: Array, Map: Types.TMap) -> bool:
	if Params.size() != 1:
		return false
	Map.Caption = Params[0]
	return true


func _ProcessMapBrickDensity(Params: Array, Map: Types.TMap) -> bool:
	if Params.size() != 1:
		return false
	var v = Params[0].to_int()
	Map.BrickDensity = clamp(v, 0, 100)
	return true


func _ProcessMapFields(Params: Array, Map: Types.TMap) -> bool:
	if Params.size() < 2:
		return false
		
	var row: int = Params[0].to_int()
	var col: int = 0
	for c in Params[1] as String:
		var type = Types.FIELD_EMPTY
		match c:
			'.': type = Types.FIELD_EMPTY
			':': type = Types.FIELD_BRICK
			'#': type = Types.FIELD_SOLID
		Map.Fields[row][col] = type
		col += 1
	return true


func _ProcessMapStartLocation(Params: Array, Map: Types.TMap) -> bool:
	if Params.size() < 3:
		return false
	
	var sp = Types.TMapStartPosition.new()
	sp.Slot = Params[0].to_int()
	sp.Pos.x = Params[1].to_int()
	sp.Pos.y = Params[2].to_int()
	sp.Primary = true
	if Params.size() >= 4:
		sp.Primary = Params[3].to_int() == 0
	Map.StartPositions[sp.Slot] = sp
	return true


func _ProcessMapPowerups(Params: Array, Map: Types.TMap):
	if Params.size() < 5:
		return false
		
	var pu = Types.TMapPowerUp.new()
	pu.Number = Params[0].to_int()
	pu.BornWith = Params[1].to_int() != 0
	pu.HasOverride = Params[2].to_int() != 0
	pu.OverrideValue = Params[3].to_int()
	pu.Forbidden = Params[4].to_int() != 0
	Map.PowerUps[pu.Number] = pu
	return true


func _ReadMapFileLine(Line: String, Map: Types.TMap):
	var s: String = Line.strip_edges()
	if s.is_empty():
		return true
	if s.begins_with(";") || !s.begins_with("-"):	# ignore comments (start with ";") and anything that's not a command (doesn't start with "-")
		return true
		
	var cmd: String = _ReadCommandFromSchemeLine(s)
	var params: Array = _ReadParamsFromSchemeLine(s)
	var r: bool = false
	match cmd:
		"-v": r = _ProcessMapVersion(params, Map)
		"-n": r = _ProcessMapName(params, Map)
		"-b": r = _ProcessMapBrickDensity(params, Map)
		"-r": r = _ProcessMapFields(params, Map)
		"-s": r = _ProcessMapStartLocation(params, Map)
		"-p": r = _ProcessMapPowerups(params, Map)
	return r


func LoadMap(mapname: String) -> Types.TMap:
	var contents = _ReadMapFileContents(mapname)
	if contents.is_empty():
		return null
		
	var map = Types.TMap.new()
	map.initialize(mapname, Types.MAP_WIDTH, Types.MAP_HEIGHT)
	var lines = contents.split("\n")
	for s in lines:
		_ReadMapFileLine(s, map)
	return map


func _IsValidFieldPos(Map: Types.TMap, Pos: Vector2i) -> bool:
	return (Pos.x >= 0) && (Pos.x < Map.Width) && (Pos.y >= 0) && (Pos.y < Map.Height)


func SetFieldTypeTo(Map: Types.TMap, Pos: Vector2i, Value: int) -> void:
	if _IsValidFieldPos(Map, Pos):
		Map.Fields[Pos.y][Pos.x] = Value
	pass
	

func GetFieldType(Map: Types.TMap, Pos: Vector2i) -> int:
	if _IsValidFieldPos(Map, Pos):
		return Map.Fields[Pos.y][Pos.x]
	return -1


func ClearField(Map: Types.TMap, Pos: Vector2i) -> void:
	if _IsValidFieldPos(Map, Pos):
		if Map.Fields[Pos.y][Pos.x] == Types.FIELD_BRICK:
			Map.Fields[Pos.y][Pos.x] = Types.FIELD_EMPTY
	pass


func ClearFieldsAround(Map: Types.TMap, Pos: Vector2i) -> void:
	ClearField(Map, Vector2i(Pos.x, Pos.y)) # pos
	ClearField(Map, Vector2i(Pos.x, Pos.y - 1)) # above
	ClearField(Map, Vector2i(Pos.x, Pos.y + 1)) # below
	ClearField(Map, Vector2i(Pos.x - 1, Pos.y)) # left
	ClearField(Map, Vector2i(Pos.x + 1, Pos.y)) # right
	pass
