extends Node2D


func _on_new_lobby_btn_pressed() -> void:
	Server.Start()
	pass


func _on_stop_lobby_btn_pressed() -> void:
	Server.Stop()
	pass


func _on_leave_lobby_btn_pressed() -> void:
	Server.Disconnect()
	pass


func _on_join_lobby_btn_pressed() -> void:
	Server.Connect("127.0.0.1")
	pass
