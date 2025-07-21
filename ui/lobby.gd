extends TScene


signal OnLeaveLobby


var Map: Types.TMap
@onready var Maps: TMaps = TMaps.new()


func BeforeShow() -> void:
	super()
	Client.OnDisconnectedFromServer.connect(_client_disconnected_from_server)
	Client.OnPlayerJoined.connect(_client_player_joined)
	Client.OnPlayerLeft.connect(_client_player_left)
	Client.OnPlayerMovedToSlot.connect(_client_player_moved_to_slot)
	Client.OnMapNameChanged.connect(_client_map_name_changed)
	pass

func AfterHide() -> void:
	super()
	Client.OnDisconnectedFromServer.disconnect(_client_disconnected_from_server)
	Client.OnPlayerJoined.disconnect(_client_player_joined)
	Client.OnPlayerLeft.disconnect(_client_player_left)
	Client.OnPlayerMovedToSlot.disconnect(_client_player_moved_to_slot)
	Client.OnMapNameChanged.disconnect(_client_map_name_changed)
	pass


func _process(_delta: float) -> void:
	if visible:
		_HandleUserInput()
	pass


func _HandleUserInput() -> void:
	if Input.is_action_just_pressed("ui_cancel"):
		_LeaveLobby()
	pass


func _client_disconnected_from_server() -> void:
	OnLeaveLobby.emit()
	pass


func _client_player_joined(_PlayerID: int) -> void:
	_UpdatePlayerInfo()
	pass
	
func _client_player_left(_PlayerID: int) -> void:
	_UpdatePlayerInfo()
	pass
	
func _client_player_moved_to_slot(_PlayerID: int, _SlotIndex: int) -> void:
	_UpdatePlayerInfo()
	pass


func _client_map_name_changed(MapName: String) -> void:
	_SelectMap(MapName)
	pass


func _UpdateControlVisibility() -> void:
	if visible && Network.IsConnected():
		$StartButton.visible = Network.IsServer()
		$MapsList.visible = Network.IsServer()
	pass


func _LeaveLobby() -> void:
	if Network.IsConnected():
		Client.Disconnect()
		if Network.IsServer():
			Server.Stop()
	pass


func _FindPlayerNameLabel(Slot: int) -> Node:
	var node = $PlayerInfo.find_child(str(Slot + 1))
	if node:
		var c = node.find_child("NameLabel")
		if c:
			return c
	return null


func _FindPlayerForSlot(SlotIndex: int) -> Types.TPlayer:
	var main = get_tree().get_root().get_node("Main")
	if !main:
		return null
	if (SlotIndex < 0) || (SlotIndex >= Network.MAX_CLIENTS):
		return null
	return Client.Data.Slots[SlotIndex].Player


func _UpdatePlayerInfo() -> void:
	for i in 10:
		var label = _FindPlayerNameLabel(i)
		label.text = "..."
		var player: Types.TPlayer = _FindPlayerForSlot(i)
		if player:
			if player.PeerID != 0:
				label.text = str(player.PeerID)
	pass


func _UpdateClientInfo() -> void:
	var s = "..."
	if multiplayer:
		s = "id: " + str(multiplayer.get_unique_id())
	$ClientInfoLabel.text = s
	pass


func _UpdateMapInfo() -> void:
	$MapNameLabel.text = Client.CurrentMapName
	#if context.map:
		#SelectMap(context.map.name) # reload map when returning to lobby
	#FillMapsList()
	pass


func _FillMapsList() -> void:
	Maps.FindMapFiles()
	
	var selname = ""
	if Map:
		selname = Map.Name
	var selidx = 0
	
	$MapsList.clear()
	for s in Maps.MapNames:
		var idx = $MapsList.add_item(s)
		if s == selname:
			selidx = idx
			
	$MapsList.select(selidx)
	pass


func _on_leave_button_pressed() -> void:
	_LeaveLobby()
	pass


func _on_start_button_pressed() -> void:
	Client.RequestStartMatch()
	pass


func _on_visibility_changed() -> void:
	if visible:
		_UpdateControlVisibility()
		_UpdatePlayerInfo()
		_UpdateClientInfo()
		_UpdateMapInfo()
		_FillMapsList()
	pass


func _SelectMap(MapName: String) -> void:
	Map = Maps.LoadMap(MapName)
	_UpdateMapInfo()
	queue_redraw()
	pass


func _draw() -> void:
	if Map:
		var offset = Vector2($MapNameLabel.position.x, $MapNameLabel.position.y + $MapNameLabel.size.y + 10)
		const FIELDWIDTH = 16
		const FIELDHEIGHT = 15
		for row in Map.Fields.size():
			for col in Map.Fields[row].size():
				var type = Map.Fields[row][col]
				var color: Color
				match type:
					Types.FIELD_EMPTY: color = Color.GREEN
					Types.FIELD_SOLID: color = Color.WEB_GRAY
					Types.FIELD_BRICK: color = Color.RED
				var r = Rect2(col * FIELDWIDTH + offset.x, row * FIELDHEIGHT + offset.y, FIELDWIDTH, FIELDHEIGHT)
				draw_rect(r, color)
	pass


func _on_maps_list_item_selected(Index: int) -> void:
	var mapname = $MapsList.get_item_text(Index)
	Client.RequestMapChange(mapname)
	pass
