extends Node

var CurrentScene: TScene


func ShowAlert(text: String, title: String = "") -> void:
	var dlg = AcceptDialog.new()
	
	dlg.dialog_text = text
	dlg.title = title
	dlg.connect("confirmed", dlg.queue_free)
	dlg.connect("canceled", dlg.queue_free)
	
	var scene_tree = Engine.get_main_loop()
	scene_tree.current_scene.add_child(dlg)
	
	dlg.popup_centered()
	pass


func SwitchToScene(Scene: TScene) -> void:
	if is_instance_valid(CurrentScene):
		CurrentScene.hide()
		CurrentScene.AfterHide()
	CurrentScene = Scene
	CurrentScene.BeforeShow()
	CurrentScene.show()
	pass


const FIELD_SIZE = Vector2(48, 45)
const FIELD_OFFSET = Vector2(64, 125)
const PLAYER_SIZE = Vector2(48, 45)

func FieldPositionToScreen(Position: Vector2) -> Vector2:
	return (Position * FIELD_SIZE) + FIELD_OFFSET

func ScreenPositionToField(Position: Vector2) -> Vector2i:
	var v: Vector2 = (Position - FIELD_OFFSET) / FIELD_SIZE
	return Vector2i(round(v.x), round(v.y))

func FieldToRect(Field: Vector2i) -> Rect2:
	var r: Rect2
	r.position = FieldPositionToScreen(Field)
	r.size = FIELD_SIZE
	return r

func PlayerPositionToRect(Position: Vector2) -> Rect2:
	return Rect2(Position.x, Position.y, PLAYER_SIZE.x, PLAYER_SIZE.y)


func FieldTypeIsPowerup(Type: int) -> bool:
	return (Type in range(Types.FIELD_PU_FIRST, Types.FIELD_PU_LAST + 1))

func FieldIsEmpty(Type: int) -> bool:
	return (Type == Types.FIELD_EMPTY) || FieldTypeIsPowerup(Type)

func FieldCanExplode(Type: int) -> bool:
	return (Type == Types.FIELD_BRICK) || FieldIsEmpty(Type)
