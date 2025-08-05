extends StaticBody2D

class_name TBombScene

var PlayerID: int
var Type: int

func LoadSpriteFromFile(FileName: String) -> void:
	$Sprite2D.texture = load(FileName)
	pass
