extends Node2D


func _ready() -> void:
	Client.OnDisconnectedFromServer.connect(_client_disconnected_from_server)
	Client.OnConnectionToServerFailed.connect(_client_connection_failed)
	pass


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


func _client_disconnected_from_server() -> void:
	Tools.ShowAlert("Disconnected")
	pass


func _client_connection_failed() -> void:
	Tools.ShowAlert("Failed to connect")
	pass
