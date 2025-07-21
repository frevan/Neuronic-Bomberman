extends Node2D


var Server: TServer
var Client: TClient


func _ready() -> void:
	Server = TServer.new()
	add_child(Server)
	Client = TClient.new()
	add_child(Client)
	
	$Menu.Initialize(Server, Client)
	Tools.SwitchToScene($Menu)
	pass
