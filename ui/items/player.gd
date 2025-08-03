extends CharacterBody2D


const SPEED = 400.0
const SPEED_STEP = 200.0



signal CheckForCollisions(Sender: Node2D, NewPosition: Vector2)


@export var animation = ""

var Data: TGameData
var SlotIndex: int = Constants.INVALID_SLOT

@onready var Rules: TRules = TRules.new()


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
	_MoveInSteps(delta)
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


func _CalculatePlayerSpeed(delta: float) -> int:
	var _speed = Constants.SPEED_MIN
	if Data:
		var idx = Data.FindSlotForPlayer(Network.PeerID)
		if idx != Constants.INVALID_SLOT:
			var slot: TSlot = Data.Slots[idx]
			_speed = slot.Player.Speed
	_speed = _ApplyDiseasesToSpeed(delta, _speed)
	return SPEED * _speed * 0.25

func _MoveInSteps(delta: float) -> void:
	var spd = _CalculatePlayerSpeed(delta)
	_CalculateVelocity(spd)
	var step_delta = delta / (spd / SPEED_STEP)
	var cur_delta = 0
	while cur_delta < delta:
		if !velocity == Vector2.ZERO:
			_Move(step_delta)
		cur_delta += step_delta
	pass

func _CalculateVelocity(CurrentSpeed: float) -> void:
	if $PlayerInput.direction:
		velocity.x = $PlayerInput.direction.x * CurrentSpeed
		velocity.y = $PlayerInput.direction.y * CurrentSpeed
	else:
		velocity = Vector2.ZERO
	pass

func _Move(delta: float) -> void:
	var newpos: Vector2 = Vector2(position.x + (velocity.x * delta),  position.y + (velocity.y * delta))
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


func _on_player_input_direction_changed(Delta: float) -> void:
	if SlotIndex != Constants.INVALID_SLOT:
		var slot: TSlot = Data.Slots[SlotIndex]
		if slot.Player.Diseases[Constants.DISEASE_REVERSE_CONTROLS]:
			$PlayerInput.direction = Rules.ProcessDisease_ReverseControls(Delta, $PlayerInput.direction)
		if slot.Player.Diseases[Constants.DISEASE_RANDOM_MOVEMENT]:
			$PlayerInput.direction = Rules.ProcessDisease_RandomMovement(Delta, $PlayerInput.direction)
	pass


func _ApplyDiseasesToSpeed(Delta: float, Speed: float) -> float:
	var spd: float = Speed
	if SlotIndex != Constants.INVALID_SLOT:
		var slot: TSlot = Data.Slots[SlotIndex]
		if slot.Player.Diseases[Constants.DISEASE_SLOW]:
			spd = Rules.ProcessDisease_Slow(Delta, spd)
		if slot.Player.Diseases[Constants.DISEASE_STICKY_MOVEMENT]:
			spd = Rules.ProcessDisease_StickyMovement(Delta, spd)
	return spd
