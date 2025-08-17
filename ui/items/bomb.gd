extends CharacterBody2D

class_name TBombScene


var PlayerID: int
var Type: int
var BombID: int

@export var direction = Vector2()


func LoadSpriteFromFile(FileName: String) -> void:
	$Sprite2D.texture = load(FileName)
	pass


func _physics_process(_delta: float) -> void:
	if !visible:
		return
	if get_multiplayer_authority() == Network.PeerID:
		if direction != Vector2.ZERO:
			velocity = direction * Constants.BOMB_SPEED
			move_and_slide()
	pass
