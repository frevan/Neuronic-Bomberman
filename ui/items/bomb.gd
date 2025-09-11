extends CharacterBody2D

class_name TBombScene


signal Collided(Sender: TBombScene)


var PlayerID: int
var PlayerSlotIndex: int
var Type: int
var BombID: int

@export var direction = Vector2()


func LoadSpriteFromFile(FileName: String) -> void:
	$Sprite2D.texture = load(FileName)
	if PlayerSlotIndex >= 0 && PlayerSlotIndex < Colors.SPRITE_COLOR_COUNT:
		$Sprite2D.material.set_shader_parameter("new_color", Colors.SPRITE_COLORS[PlayerSlotIndex])
	pass


func EnableCollisions(Value: bool) -> void:
	$CollisionShape2D.disabled = !Value
	pass


func _physics_process(_delta: float) -> void:
	if !visible:
		return
	if get_multiplayer_authority() == Network.PeerID:
		if direction != Vector2.ZERO:
			velocity = direction * Constants.BOMB_SPEED
			var collided: bool = move_and_slide()
			if collided:
				Collided.emit(self)
	pass
