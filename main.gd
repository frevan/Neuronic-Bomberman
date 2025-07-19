extends Node2D


func _on_new_lobby_btn_pressed() -> void:
	if !Server.Start():
		Tools.ShowAlert("Can't start server")
	pass


func _on_stop_lobby_btn_pressed() -> void:
	if !Server.Stop():
		Tools.ShowAlert("Can't stop server")
	pass


func _on_leave_lobby_btn_pressed() -> void:
	if !Client.Disconnect():
		Tools.ShowAlert("Can't leave lobby")
	pass


func _on_join_lobby_btn_pressed() -> void:
	if !Client.Connect("127.0.0.1"):
		Tools.ShowAlert("Can't join lobby")
	pass
