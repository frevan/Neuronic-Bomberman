extends Node2D


func LoadSpriteFromFile(FileName: String) -> void:
	$Sprite2D.texture = load(FileName)
	pass
