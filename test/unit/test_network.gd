extends GutTest

var _network: TNetwork


func before_each():
	_network = TNetwork.new()

func after_each():
	_network.queue_free()


func test_IsNotConnected():
	assert_eq(_network.IsConnected(), false)
	pass


func test_IsNotServer():
	assert_eq(_network.IsServer(), false)
	pass
