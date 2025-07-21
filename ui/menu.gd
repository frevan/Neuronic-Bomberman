extends TScene


signal OnSwitchToLobby


func BeforeShow() -> void:
	super()
	Client.OnConnectedToServer.connect(_client_connected_to_server)
	Client.OnConnectionToServerFailed.connect(_client_connection_to_server_failed)
	pass

func AfterHide() -> void:
	super()
	Client.OnConnectedToServer.disconnect(_client_connected_to_server)
	Client.OnConnectionToServerFailed.disconnect(_client_connection_to_server_failed)
	pass


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
			OnSwitchToLobby.emit()
			return
	Tools.ShowAlert("Something went wrong when starting the server")
	pass

func _on_join_btn_pressed() -> void:
	Client.Connect("127.0.0.1")
	pass


func _client_connected_to_server() -> void:
	OnSwitchToLobby.emit()
	pass

func _client_connection_to_server_failed() -> void:
	Tools.ShowAlert("Failed to connect")
	pass
