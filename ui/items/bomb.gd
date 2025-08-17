extends CharacterBody2D

class_name TBombScene

var PlayerID: int
var Type: int

@export var direction = Vector2()


func LoadSpriteFromFile(FileName: String) -> void:
	$Sprite2D.texture = load(FileName)
	pass


func _physics_process(delta: float) -> void:
	if !visible:
		return
	if get_multiplayer_authority() == Network.PeerID:
		velocity = direction * 400
		move_and_slide()
	pass
