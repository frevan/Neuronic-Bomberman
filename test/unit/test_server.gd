extends GutTest

var TServer = preload("res://server/server.gd")
var _server: TServer


func before_each():
	_server = TServer.new()

func after_each():
	_server.queue_free()


func test_IsNotConnected():
	assert_eq(_server.IsConnected(), false)
	pass


func test_IsNotRunning():
	assert_eq(_server.IsRunning(), false)
	pass
