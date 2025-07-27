extends TScene


signal OnLeave
signal OnConnect


const MAX_RECENT_SERVERS = 6

var RecentServers: Array


func BeforeShow() -> void:
	super()
	Client.OnConnectionToServerFailed.connect(_client_connection_to_server_failed)
	pass

func AfterHide() -> void:
	super()
	Client.OnConnectionToServerFailed.disconnect(_client_connection_to_server_failed)
	pass


func _ready() -> void:
	pass

func _process(_delta: float) -> void:
	if visible:
		_HandleUserInput()
	pass

func _on_visibility_changed() -> void:
	if visible:
		_LoadRecentServersFromFile()
		_UpdateRecentLabel()
		if RecentServers.size() == 0:
			$ServerEdit.text = "127.0.0.1"
		else:
			$ServerEdit.text = RecentServers[0]
	pass


func _HandleUserInput():
	if Input.is_action_just_pressed("ui_cancel"):
		OnLeave.emit()
	elif Input.is_action_just_pressed("ui_accept"):
		_Connect($ServerEdit.text)
	for key in [KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5]:
		if Input.is_key_pressed(key):
			_HandleFunctionKeyPress(key)
	pass

func _HandleFunctionKeyPress(Key: int) -> void:
	var idx = (Key - KEY_F1) + 1
	if idx < RecentServers.size():
		_Connect(RecentServers[idx])
	pass


func _on_back_btn_pressed() -> void:
	OnLeave.emit()
	pass


func _on_connect_btn_pressed() -> void:
	_Connect($ServerEdit.text)
	pass


func _client_connection_to_server_failed() -> void:
	Tools.ShowAlert("Failed to connect")
	pass


func _Connect(Address: String) -> void:
	if IP.resolve_hostname(Address) == "":
		Tools.ShowAlert("This is not a valid server address")
		return
	_StoreRecentServer(Address)
	OnConnect.emit(Address)
	pass


func _UpdateRecentLabel() -> void:
	$RecentLabel.visible = RecentServers.size() > 0
	if !$RecentLabel.visible:
		return
	var lines: Array
	lines.assign(RecentServers)
	var s: String = ""
	for i in lines.size():
		if i == 0:
			continue
		if i >= MAX_RECENT_SERVERS - 1:
			break
		if s != "":
			s += "\n"
		s += "[F" + str(i) + "] " + lines[i]
	$RecentLabel.text = s
	pass


func _StoreRecentServer(Address: String) -> void:
	var idx = RecentServers.find(Address)
	if idx != -1:
		RecentServers.remove_at(idx)
	RecentServers.insert(0, Address)
	if RecentServers.size() > MAX_RECENT_SERVERS:
		RecentServers.resize(MAX_RECENT_SERVERS)
	
	_SaveRecentServersToFile()
	_UpdateRecentLabel()
	pass

func _SaveRecentServersToFile() -> void:
	var fname: String = "user://recent.txt"
	var file = FileAccess.open(fname, FileAccess.WRITE)
	if !file:
		return
	var contents = "\n".join(RecentServers)
	file.store_string(contents)
	file.close()
	pass

func _LoadRecentServersFromFile() -> void:
	RecentServers.clear()
	
	var fname: String = "user://recent.txt"
	if !FileAccess.file_exists(fname):
		return
	var file = FileAccess.open(fname, FileAccess.READ)
	if !file:
		return
	var contents = file.get_as_text(true)
	file.close()
	
	RecentServers = contents.split("\n")
	if RecentServers.size() > MAX_RECENT_SERVERS:
		RecentServers.resize(MAX_RECENT_SERVERS)
	pass
