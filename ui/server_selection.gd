extends TScene


signal OnLeave
signal OnConnect


func BeforeShow() -> void:
	super()
	Client.OnConnectionToServerFailed.connect(_client_connection_to_server_failed)
	pass

func AfterHide() -> void:
	super()
	Client.OnConnectionToServerFailed.disconnect(_client_connection_to_server_failed)
	pass


func _ready() -> void:
	$ServerEdit.text = "127.0.0.1"
	pass

func _process(_delta: float) -> void:
	if visible:
		_HandleUserInput()
	pass


func _HandleUserInput():
	if Input.is_action_just_pressed("ui_cancel"):
		OnLeave.emit()
	elif Input.is_action_just_pressed("ui_accept"):
		_Connect()
	pass


func _on_back_btn_pressed() -> void:
	OnLeave.emit()
	pass


func _on_connect_btn_pressed() -> void:
	_Connect()
	pass


func _client_connection_to_server_failed() -> void:
	Tools.ShowAlert("Failed to connect")
	pass


func _Connect() -> void:
	var address: String = $ServerEdit.text
	if IP.resolve_hostname(address) == "":
		Tools.ShowAlert("This is not a valid server address")
		return
	OnConnect.emit($ServerEdit.text)
	pass
