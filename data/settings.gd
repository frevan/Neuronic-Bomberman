extends Node

var PlayerName: String = ""
var FXVolume: float = 1.0


func _ReadFileContents(FileName: String) -> String:
	var fname = FileName
	if !FileAccess.file_exists(fname):
		return ""
	var file = FileAccess.open(fname, FileAccess.READ)
	if !file:
		return ""
	var result = file.get_as_text(true)
	file.close()
	return result

func _WriteContentsToFile(FileName: String, Contents: String) -> void:
	var file = FileAccess.open(FileName, FileAccess.WRITE)
	if !file:
		return
	file.store_string(Contents)
	file.close()

func _AddFloatOption(Contents: String, Name: String, Value: float) -> String:
	if Contents != "":
		Contents = Contents + "\n"
	Contents = Contents + Name + "=" + str(Value)
	return Contents

func _ReadFloatOption(Value: String, Default: float) -> float:
	if Value.is_valid_float():
		return Value.to_float()
	return Default


func _ReadFileLine(Line: String) -> void:
	var s: String
	var a = Line.split("#", true, 1) # remove comment
	if a.size() >= 1:
		s = a[0]
	s = s.strip_edges()
	if s.is_empty():
		return
	var line_parts = s.split("=")
	if line_parts.size() == 0:
		return
	match line_parts[0]:
		"fxvolume": FXVolume = _ReadFloatOption(line_parts[1], 1.0)


func _ReadPlayerNameFromSettings() -> String:
	var s: String = _ReadFileContents("user://playername.txt")
	return s

func _SavePlayerNameToSettings(Name: String) -> void:
	_WriteContentsToFile("user://playername.txt", Name)


func _ReadOptionsFromFile() -> void:
	var contents: String = _ReadFileContents("user://settings.txt")
	var lines = contents.split("\n")
	for s in lines:
		_ReadFileLine(s)

func _WriteOptionsToFile() -> void:
	var contents: String = ""
	contents = _AddFloatOption(contents, "fxvolume", FXVolume)
	_WriteContentsToFile("user://settings.txt", contents)


func SaveSettings() -> void:
	_SavePlayerNameToSettings(PlayerName)
	_WriteOptionsToFile()

func LoadSettings() -> void:
	PlayerName = _ReadPlayerNameFromSettings()
	_ReadOptionsFromFile()
