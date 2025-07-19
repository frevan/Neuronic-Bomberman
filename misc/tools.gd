extends Node

func ShowAlert(text: String, title: String = "") -> void:
	var dlg = AcceptDialog.new()
	
	dlg.dialog_text = text
	dlg.title = title
	dlg.connect("confirmed", queue_free)
	dlg.connect("canceled", queue_free)
	
	var scene_tree = Engine.get_main_loop()
	scene_tree.current_scene.add_child(dlg)
	
	dlg.popup_centered()
	pass
