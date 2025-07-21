extends Node2D


var Server: TServer
var Client: TClient


func _ready() -> void:
	_InitializeServer()
	_InitializeClient()
	
	$Menu.Initialize(Server, Client)
	$Lobby.Initialize(Server, Client)
	$Connecting.Initialize(Server, Client)
	
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
	pass


func _client_connected_to_server() -> void:
	Tools.SwitchToScene($Lobby)
	pass


func _client_connection_to_server_failed() -> void:
	Tools.ShowAlert("Failed to connect")
	Tools.SwitchToScene($Menu)
	pass


func _on_menu_on_connecting() -> void:
	Tools.SwitchToScene($Connecting)
	pass
