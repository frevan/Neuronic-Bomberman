extends Node2D


var Server: TServer
var Client: TClient

var OldWindowMode


func _ready() -> void:
	if OS.has_feature("debug"):
		_PositionWindowForDebug()
	
	_InitializeServer()
	_InitializeClient()

	_InitializeMenuScene()
	_InitializeLobbyScene()
	$Connecting.Initialize(Client)
	_InitializeMatchScene()
	_InitializeScoreScene()
	_InitializeServerSelectionScene()
	
	Tools.SwitchToScene($Menu)
	pass


func _unhandled_input(event: InputEvent) -> void:
	if event is InputEventKey:
		if event.pressed and event.keycode == KEY_ENTER and event.alt_pressed:
			if DisplayServer.window_get_mode() == DisplayServer.WINDOW_MODE_FULLSCREEN:
				DisplayServer.window_set_mode(OldWindowMode)
			else:
				OldWindowMode = DisplayServer.window_get_mode()
				DisplayServer.window_set_mode(DisplayServer.WINDOW_MODE_FULLSCREEN)
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
	Client.OnRoundEnded.connect(_client_round_ended)
	Client.OnMatchEnded.connect(_client_match_ended)
	Client.OnPlayerPositionUpdated.connect(_client_player_position_updated)
	Client.OnBombPositionUpdated.connect(_client_bomb_position_updated)
	pass

func _InitializeMenuScene() -> void:
	$Menu.Initialize(Client)
	$Menu.Server = Server
	$Menu.OnJoinServer.connect(_menu_join_server)
	pass

func _InitializeLobbyScene() -> void:
	$Lobby.Initialize(Client)
	$Lobby.OnDisconnectAndStop.connect(_lobby_disconnect_and_stop)
	pass

func _InitializeMatchScene() -> void:
	$Match.Initialize(Client)
	$Match.OnLeaveLobby.connect(_match_leave_lobby)
	pass

func _InitializeScoreScene() -> void:
	$Score.Initialize(Client)
	$Score.OnLeaveMatch.connect(_score_leave_match)
	$Score.OnReadyForNextRound.connect(_score_ready_for_next_round)
	pass

func _InitializeServerSelectionScene() -> void:
	$ServerSelection.Initialize(Client)
	$ServerSelection.OnConnect.connect(_server_selection_connect)
	$ServerSelection.OnLeave.connect(_server_selection_leave)
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
	if !$Score.visible:
		Tools.SwitchToScene($Match)
		$Match.AfterShow()
		Client.RequestPlayerReady(true)
	pass

func _client_round_ended() -> void:
	Tools.SwitchToScene($Score)
	pass
	
func _client_match_ended() -> void:
	Tools.SwitchToScene($Score)
	pass

func _client_player_position_updated(ID: int, Position: Vector2) -> void:
	if Server:
		Server.UpdatePlayerPosition(ID, Position)
	pass

func _client_bomb_position_updated(BombID: int, Position: Vector2) -> void:
	if Server:
		Server.UpdateBombPosition(BombID, Position)
	pass


func _menu_join_server() -> void:
	Tools.SwitchToScene($ServerSelection)
	pass


func _lobby_disconnect_and_stop() -> void:
	_DisconnectAndStop()
	Tools.SwitchToScene($Menu)
	pass


func _match_leave_lobby() -> void:
	_DisconnectAndStop()
	Tools.SwitchToScene($Menu)
	pass


func _score_leave_match() -> void:
	Tools.SwitchToScene($Lobby)
	pass

func _score_ready_for_next_round() -> void:
	Tools.SwitchToScene($Match)
	$Match.AfterShow()
	Client.RequestPlayerReady(true)
	pass


func _server_selection_connect(Address: String) -> void:
	Client.Connect(Address)
	Tools.SwitchToScene($Connecting)
	pass

func _server_selection_leave() -> void:
	Tools.SwitchToScene($Menu)
	pass


func _DisconnectAndStop() -> void:
	if Network.IsConnected():
		Client.Disconnect()
		if Network.IsServer():
			Server.Stop()
	pass
