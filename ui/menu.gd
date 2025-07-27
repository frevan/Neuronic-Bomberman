extends TScene


signal OnJoinServer


var Server: TServer


func BeforeShow() -> void:
	super()
	_ReadPlayerNameFromSettings()
	$NameEdit.text = Client.PlayerName
	pass


func AfterHide() -> void:
	_SavePlayerNameToSettings()
	super()
	pass


func _on_host_btn_pressed() -> void:
	Client.PlayerName = $NameEdit.text
	if Server.Start():
		if Client.Connect("127.0.0.1"):
			return
	Tools.ShowAlert("Something went wrong when starting the server")
	pass

func _on_join_btn_pressed() -> void:
	Client.PlayerName = $NameEdit.text
	OnJoinServer.emit()
	pass


func _ReadPlayerNameFromSettings() -> void:
	Client.PlayerName = ""
	var fname: String = "user://playername.txt"
	if !FileAccess.file_exists(fname):
		return
	var file = FileAccess.open(fname, FileAccess.READ)
	if !file:
		return
	Client.PlayerName = file.get_as_text(true)
	file.close()
	pass


func _SavePlayerNameToSettings() -> void:
	var fname: String = "user://playername.txt"
	var file = FileAccess.open(fname, FileAccess.WRITE)
	if !file:
		return
	file.store_string(Client.PlayerName)
	file.close()
	pass


func _on_quit_btn_pressed() -> void:
	get_tree().root.propagate_notification(NOTIFICATION_WM_CLOSE_REQUEST)
	get_tree().quit()
	pass
