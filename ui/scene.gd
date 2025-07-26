extends Node2D

class_name TScene


var Client: TClient


func Initialize(aClient: TClient) -> void:
	Client = aClient
	pass


func BeforeShow() -> void:
	pass


func AfterHide() -> void:
	pass
