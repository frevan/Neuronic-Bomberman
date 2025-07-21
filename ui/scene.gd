extends Node2D

class_name TScene


var Server: TServer
var Client: TClient


func Initialize(aServer: TServer, aClient: TClient) -> void:
	Server = aServer
	Client = aClient
	pass
