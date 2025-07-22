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

const FIELD_WIDTH = 48
const FIELD_HEIGHT = 45
const FIELD_OFFSET = Vector2(64, 125)

func FieldToPosition(pos: Vector2i) -> Vector2:
	return Vector2(FIELD_OFFSET.x + (pos.x * FIELD_WIDTH), FIELD_OFFSET.y + (pos.y * FIELD_HEIGHT))
	
func PositionToField(pos: Vector2) -> Vector2i:
	var x: float = (pos.x - FIELD_OFFSET.x) / FIELD_WIDTH
	var y: float = (pos.y - FIELD_OFFSET.y) / FIELD_HEIGHT
	return Vector2i(round(x), round(y))
