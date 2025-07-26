extends TScene


signal OnJoinServer


var Server: TServer


func _process(_delta: float) -> void:
	if visible:
		_HandleUserInput()
	pass


func _HandleUserInput():
	if Input.is_action_just_pressed("ui_cancel"):
		get_tree().root.propagate_notification(NOTIFICATION_WM_CLOSE_REQUEST)
		get_tree().quit()
	pass


func _on_host_btn_pressed() -> void:
	if Server.Start():
		if Client.Connect("127.0.0.1"):
			return
	Tools.ShowAlert("Something went wrong when starting the server")
	pass

func _on_join_btn_pressed() -> void:
	OnJoinServer.emit()
	pass
