extends Node2D


var TServer = preload("res://server/server.gd")
var Server: TServer


func _ready() -> void:
	Server = TServer.new()
	add_child(Server)
	
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


func _on_move_to_next_slot_btn_pressed() -> void:
	assert(is_instance_valid(Client.Data))
	var idx: int = Client.Data.FindSlotForPlayer(Network.PeerID)
	if idx != Types.INVALID_SLOT:
		var new_idx = (idx + 1) % Client.Data.Slots.size()
		#if new_idx >= Client.Data.Slots.size():
		#	new_idx = 0
		Client.MovePlayerToSlot(new_idx)
	pass


func _client_disconnected_from_server() -> void:
	Tools.ShowAlert("Disconnected")
	pass


func _client_connection_failed() -> void:
	Tools.ShowAlert("Failed to connect")
	pass
