extends Node2D


func _on_new_lobby_btn_pressed() -> void:
	Server.StartServer()
	pass


func _on_leave_lobby_btn_pressed() -> void:
	Server.StopServer()
	pass
