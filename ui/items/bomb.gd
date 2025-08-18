extends CharacterBody2D

class_name TBombScene


signal StoppedMoving(Sender: TBombScene)


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
			var collided: bool = move_and_slide()
			if collided:
				direction = Vector2.ZERO
				var field = Tools.ScreenPositionToField(position)
				position = Tools.FieldPositionToScreen(field) + Tools.BOMB_POS_OFFSET
				StoppedMoving.emit(self)
	pass
