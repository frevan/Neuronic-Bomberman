extends Node2D


var Server: TServer
var Client: TClient


func _ready() -> void:
	_PositionWindowForDebug()
	
	_InitializeServer()
	_InitializeClient()
	
	$Menu.Initialize(Client)
	$Menu.Server = Server
	_InitializeLobbyScene()
	$Connecting.Initialize(Client)
	_InitializeMatchScene()
	
	Tools.SwitchToScene($Menu)
	pass


func _on_lobby_on_leave_lobby() -> void:
	Tools.SwitchToScene($Menu)
	pass


func _PositionWindowForDebug() -> void:
	var args: PackedStringArray = OS.get_cmdline_args()
	var window: Window = get_window()
	for s: String in args:
		match s:
			#"--inst1": window.position = window.position - Vector2i(200, 0)
			"--inst2": window.position = window.position + Vector2i(400, 200)
	pass


func _InitializeServer() -> void:
	Server = TServer.new()
	add_child(Server)
	pass

func _InitializeClient() -> void:
	Client = TClient.new()
	add_child(Client)
	
	Client.OnConnectedToServer.connect(_client_connected_to_server)
	Client.OnConnectionToServerFailed.connect(_client_connection_to_server_failed)
	Client.OnDisconnectedFromServer.connect(_client_disconnected_from_server)
	Client.OnMatchStarted.connect(_client_match_started)
	Client.OnNewRound.connect(_client_new_round)
	Client.OnPlayerPositionUpdated.connect(_client_player_position_updated)
	pass

func _InitializeLobbyScene() -> void:
	$Lobby.Initialize(Client)
	$Lobby.OnDisconnectAndStop.connect(_lobby_disconnect_and_stop)
	pass

func _InitializeMatchScene() -> void:
	$Match.Initialize(Client)
	$Match.OnLeaveLobby.connect(_match_leave_lobby)
	pass


func _client_connected_to_server() -> void:
	Tools.SwitchToScene($Lobby)
	pass

func _client_connection_to_server_failed() -> void:
	Tools.ShowAlert("Failed to connect")
	Tools.SwitchToScene($Menu)
	pass

func _client_disconnected_from_server() -> void:
	Tools.SwitchToScene($Menu)
	pass

func _client_match_started() -> void:
	# TODO: show intermediate screen?
	pass

func _client_new_round() -> void:
	Tools.SwitchToScene($Match)
	$Match.UpdateAll()
	pass

func _client_player_position_updated(ID: int, Position: Vector2) -> void:
	if Server:
		Server.UpdatePlayerPosition(ID, Position)
	pass


func _on_menu_on_connecting() -> void:
	Tools.SwitchToScene($Connecting)
	pass


func _lobby_disconnect_and_stop() -> void:
	_DisconnectAndStop()
	Tools.SwitchToScene($Menu)
	pass


func _match_leave_lobby() -> void:
	_DisconnectAndStop()
	Tools.SwitchToScene($Menu)
	pass



func _DisconnectAndStop() -> void:
	if Network.IsConnected():
		Client.Disconnect()
		if Network.IsServer():
			Server.Stop()
	pass
