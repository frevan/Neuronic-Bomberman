extends CharacterBody2D

class_name TPlayerScene


signal OnDirectionChanged(Sender: Node2D, NewDirection: Vector2)
signal CollidedWithBomb(Sender: Node2D, Bomb: Node2D, Collision: KinematicCollision2D)


const ColorShader = "res://ui/items/player.gdshader"

@export var animation = ""

var Data: TGameData
var SlotIndex: int = Constants.INVALID_SLOT
var SpriteColor: Color

var CurrentSpriteColorIndex: int
var TimeSinceLastSpriteColorChange: float
var IsShowingDiseaseColors: bool = false
const TIME_BETWEEN_DISEASE_COLORS = 0.25 # in seconds
const SPRITE_COLOR_COUNT = 10
const SPRITE_COLORS = [Color(0x23f723ff), # green
						Color(0xff0000ff), # red
						Color(0xff00ffff), # pink
						Color(0x00ffffff), # cyan
						Color(0x0000ffff), # blue
						Color(0x000000ff), # black
						Color(0xffff00ff), # yellow
						Color(0xff8000ff), # orange
						Color(0x9900a5ff), # purple
						Color(0xffffffff)] # white

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

func _process(delta: float) -> void:
	if !visible:
		return
	_StartOrStopAnimation()
	_UpdateSpriteColorForDisease(delta)
	pass

func _physics_process(delta: float) -> void:
	if !visible:
		return
	_UpdateSpriteAnimation()
	_ForcePlayerIntoFieldWhenMoving()
	velocity = $PlayerInput.direction * _CalculatePlayerSpeed(delta)
	if velocity != Vector2.ZERO:
		var collided: bool = move_and_slide()
		if collided:
			var collision: KinematicCollision2D = get_last_slide_collision()
			var collider: Object = collision.get_collider()
			if (collider is Node2D) && (collider is TBombScene):
				CollidedWithBomb.emit(self, collider, collision)
	pass


func _InitializeSprite(_id: int) -> void:
	$AnimatedSprite.animation = "down"
	pass

func CreateSpriteShader() -> void:
	var _material: ShaderMaterial = ShaderMaterial.new()
	$AnimatedSprite.material = _material
	$AnimatedSprite.material.shader = load(ColorShader)
	$AnimatedSprite.material.set_shader_parameter("tolerance", 1.0)
	$AnimatedSprite.material.set_shader_parameter("prev_color", Color(0x23f723ff))
	SpriteColor = SPRITE_COLORS[SlotIndex]
	_SetSpriteColor(SpriteColor)
	pass

func _UpdateSpriteColorForDisease(Delta: float) -> void:
	if SlotIndex == Constants.INVALID_SLOT:
		return
	var slot: TSlot = Data.Slots[SlotIndex]
	var has_disease: bool = slot.Player.HasDisease()
	if has_disease != IsShowingDiseaseColors:
		IsShowingDiseaseColors = has_disease
		if !has_disease:
			_SetSpriteColor(SpriteColor)
		else:
			TimeSinceLastSpriteColorChange = TIME_BETWEEN_DISEASE_COLORS
			CurrentSpriteColorIndex = SlotIndex
	if IsShowingDiseaseColors:
		if TimeSinceLastSpriteColorChange >= TIME_BETWEEN_DISEASE_COLORS:
			TimeSinceLastSpriteColorChange = 0
			CurrentSpriteColorIndex += 1
			if CurrentSpriteColorIndex >= SPRITE_COLOR_COUNT:
				CurrentSpriteColorIndex = 0
			_SetSpriteColor(SPRITE_COLORS[CurrentSpriteColorIndex])
		else:
			TimeSinceLastSpriteColorChange += Delta
	pass

func _SetSpriteColor(C: Color) -> void:
	$AnimatedSprite.material.set_shader_parameter("new_color", C)
	queue_redraw()
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
	return Constants.PLAYER_SPEED * _speed * 0.25


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
		if slot.Player.Diseases[Constants.DISEASE_REVERSE_CONTROLS] > 0:
			$PlayerInput.direction = Rules.ProcessDisease_ReverseControls(Delta, $PlayerInput.direction)
		if slot.Player.Diseases[Constants.DISEASE_RANDOM_MOVEMENT] > 0:
			$PlayerInput.direction = Rules.ProcessDisease_RandomMovement(Delta, $PlayerInput.direction)
	pass


func _ApplyDiseasesToSpeed(Delta: float, Speed: float) -> float:
	var spd: float = Speed
	if SlotIndex != Constants.INVALID_SLOT:
		var slot: TSlot = Data.Slots[SlotIndex]
		if slot.Player.Diseases[Constants.DISEASE_SLOW] > 0:
			spd = Rules.ProcessDisease_Slow(Delta, spd)
		if slot.Player.Diseases[Constants.DISEASE_STICKY_MOVEMENT] > 0:
			spd = Rules.ProcessDisease_StickyMovement(Delta, spd)
	return spd


func _on_player_input_on_direction_changed(NewDirection: Vector2) -> void:
	OnDirectionChanged.emit(self, NewDirection)


func _ForcePlayerIntoFieldWhenMoving() -> void:
	if $PlayerInput.direction.x != 0:
		position.y = Tools.FieldPositionToScreen(Tools.ScreenPositionToField(position)).y
	elif $PlayerInput.direction.y != 0:
		position.x = Tools.FieldPositionToScreen(Tools.ScreenPositionToField(position)).x
	pass
