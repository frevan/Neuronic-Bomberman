extends Node2D


func _on_new_lobby_btn_pressed() -> void:
	Server.StartServer()
	pass


func _on_stop_lobby_btn_pressed() -> void:
	Server.StopServer()
	pass


func _on_leave_lobby_btn_pressed() -> void:
	Server.Disconnect()
	pass


func _on_join_lobby_btn_pressed() -> void:
	Server.Connect("127.0.0.1")
	pass
