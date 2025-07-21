extends Node2D


var Server: TServer
var Client: TClient


func _ready() -> void:
	Server = TServer.new()
	add_child(Server)
	Client = TClient.new()
	add_child(Client)
	
	$Menu.Initialize(Server, Client)
	$Lobby.Initialize(Server, Client)
	
	Tools.SwitchToScene($Menu)
	pass


func _on_lobby_on_leave_lobby() -> void:
	Tools.SwitchToScene($Menu)
	pass

func _on_menu_on_switch_to_lobby() -> void:
	Tools.SwitchToScene($Lobby)
	pass
