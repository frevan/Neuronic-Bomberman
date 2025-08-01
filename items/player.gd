extends CharacterBody2D


const SPEED = 400.0


signal CheckForCollisions(Sender: Node2D, NewPosition: Vector2)


@export var animation = ""


func _ready():
	# only process physics for the local player	
	if get_multiplayer_authority() != multiplayer.get_unique_id():
		set_physics_process(false)
	pass


func _enter_tree() -> void:
	var id = name.to_int()
	set_multiplayer_authority(id)
	_InitializeSprite(id)
	pass

func _process(_delta: float) -> void:
	if !visible:
		return
	_StartOrStopAnimation()
	pass

func _physics_process(delta: float) -> void:
	if !visible:
		return
	_CalculateVelocity()
	if !velocity == Vector2.ZERO:
		_Move(delta)
	_UpdateSpriteAnimation()
	pass


func _InitializeSprite(id: int) -> void:
	if id == 1:
		$AnimatedSprite.modulate = Color(1, 0.5, 0)
	else:
		$AnimatedSprite.modulate = Color(0, 0.5, 1)
	$AnimatedSprite.animation = "down"
	pass


func ResetSpriteDirection() -> void:
	$AnimatedSprite.animation = "down"
	pass


func _CalculateVelocity() -> void:
	if $PlayerInput.direction:
		velocity.x = $PlayerInput.direction.x * SPEED
		velocity.y = $PlayerInput.direction.y * SPEED
	else:
		velocity = Vector2.ZERO
	pass


func _Move(delta: float) -> void:
	var newpos: Vector2 = Vector2(position.x + (velocity.x * delta),  position.y + (velocity.y * delta))
	position = newpos
	#position = Rules.ApplyObstaclesToPlayerMove(context, position, newpos)
	CheckForCollisions.emit(self, newpos)
	pass

func _UpdateSpriteAnimation() -> void:
	animation = ""
	if velocity.x < 0:
		animation =  "left"
	elif velocity.x > 0:
		animation = "right"
	elif velocity.y < 0:
		animation = "up"
	elif velocity.y > 0:
		animation = "down"
	pass


func _StartOrStopAnimation() -> void:
	if animation.is_empty():
		$AnimatedSprite.stop()
	else:
		$AnimatedSprite.animation = animation
		$AnimatedSprite.play()
	pass
