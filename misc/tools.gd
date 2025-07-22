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

func FieldPositionToScreen(Position: Vector2) -> Vector2:
	return (Position * FIELD_SIZE) + FIELD_OFFSET

func ScreenPositionToField(Position: Vector2) -> Vector2i:
	return (Position - FIELD_OFFSET) / FIELD_SIZE
