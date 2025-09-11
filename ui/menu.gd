extends TScene


signal OnJoinServer
signal OnShowOptions


var Server: TServer


func BeforeShow() -> void:
	super()
	Client.PlayerName = Settings.PlayerName
	$NameEdit.text = Settings.PlayerName
	pass


func AfterHide() -> void:
	super()
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


func _on_quit_btn_pressed() -> void:
	get_tree().root.propagate_notification(NOTIFICATION_WM_CLOSE_REQUEST)
	get_tree().quit()
	pass


func _on_options_btn_pressed() -> void:
	OnShowOptions.emit()
	pass


func _on_name_edit_text_changed(new_text: String) -> void:
	Settings.PlayerName = new_text
	Client.PlayerName = new_text
	pass
