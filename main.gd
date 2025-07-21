extends Node2D


var Server: TServer
var Client: TClient


func _ready() -> void:
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
