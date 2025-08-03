extends TScene


signal OnDisconnectAndStop


var Map: Types.TMap
@onready var Maps: TMaps = TMaps.new()


func BeforeShow() -> void:
	super()
	Client.OnPlayerJoined.connect(_client_player_joined)
	Client.OnPlayerLeft.connect(_client_player_left)
	Client.OnPlayerMovedToSlot.connect(_client_player_moved_to_slot)
	Client.OnMapNameChanged.connect(_client_map_name_changed)
	Client.OnPlayerBecameReady.connect(_client_player_became_ready)
	Client.OnNumRoundsChanged.connect(_client_num_rounds_changed)
	Client.OnPlayerNameChanged.connect(_client_player_name_changed)
	_AdjustControls()
	pass

func AfterHide() -> void:
	super()
	Client.OnPlayerJoined.disconnect(_client_player_joined)
	Client.OnPlayerLeft.disconnect(_client_player_left)
	Client.OnPlayerMovedToSlot.disconnect(_client_player_moved_to_slot)
	Client.OnMapNameChanged.disconnect(_client_map_name_changed)
	Client.OnPlayerBecameReady.disconnect(_client_player_became_ready)
	Client.OnNumRoundsChanged.disconnect(_client_num_rounds_changed)
	Client.OnPlayerNameChanged.disconnect(_client_player_name_changed)
	pass


func _unhandled_input(event: InputEvent) -> void:
	if event is InputEventKey:
		if event.pressed:
			if Network.IsServer() && (event.keycode == KEY_UP || event.keycode == KEY_DOWN):
				var idx: int = _IndexOfMapInList(Client.CurrentMapName)
				if event.keycode == KEY_UP:
					idx -= 1
					if idx < 0:
						idx = $MapsList.item_count - 1
				else:
					idx += 1
					if idx >= $MapsList.item_count:
						idx = 0
				var mapname = $MapsList.get_item_text(idx)
				Client.RequestMapChange(mapname)
				$MapsList.select(idx)
				$MapsList.ensure_current_is_visible()
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

func _client_player_became_ready(_PlayerID: int, _Ready: bool) -> void:
	_UpdatePlayerInfo()
	pass

func _client_num_rounds_changed(_Value: int) -> void:
	_AdjustRoundsLabelText()
	pass

func _client_player_name_changed(_PlayerID: int, _Name: String) -> void:
	_UpdatePlayerInfo()
	pass


func _UpdateControlVisibility() -> void:
	if visible && Network.IsConnected():
		$StartButton.visible = Network.IsServer()
		$MapsList.visible = Network.IsServer()
	pass


func _LeaveLobby() -> void:
	OnDisconnectAndStop.emit()
	pass


func _FindPlayerNameLabel(Slot: int) -> Node:
	var node = $PlayerInfo.find_child(str(Slot + 1))
	if node:
		var c = node.find_child("NameLabel")
		if c:
			return c
	return null


func _UpdatePlayerInfo() -> void:
	for i in Client.Data.Slots.size():
		var label = _FindPlayerNameLabel(i)
		if !label:
			continue
		
		label.text = "..."
		var slot: TSlot = Client.Data.Slots[i]
		if slot.PlayerID != 0:
			var s: String = slot.PlayerName
			if s == "":
				s = str(slot.PlayerID)
			if slot.Ready:
				s += " (ready)"
			label.text = s
			if slot.PlayerID == Network.PeerID:
				$ReadyBox.set_pressed_no_signal(slot.Ready)
	pass


func _UpdateClientInfo() -> void:
	var s = "..."
	if multiplayer:
		s = "id: " + str(multiplayer.get_unique_id())
	$ClientInfoLabel.text = s
	pass


func _UpdateMapInfo() -> void:
	$MapNameLabel.text = Client.CurrentMapName
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


func _on_ready_box_toggled(toggled_on: bool) -> void:
	Client.RequestPlayerReady(toggled_on)
	pass


func _AdjustControls() -> void:
	$NumRoundsSpin.visible = Network.IsServer()
	_AdjustRoundsLabelText()
	pass

func _AdjustRoundsLabelText() -> void:
	$NumRoundsSpin.set_value_no_signal(Client.Data.NumRounds)
	if Network.IsServer():
		$NumRoundsLabel.text = "Rounds"
	else:
		$NumRoundsLabel.text = "Rounds: " + str(Client.Data.NumRounds)
	pass


func _on_num_rounds_spin_value_changed(value: float) -> void:
	var v: int = floor(value)
	Client.RequestNumRounds(v)
	pass


func _IndexOfMapInList(Name: String) -> int:
	for i in $MapsList.item_count:
		if $MapsList.get_item_text(i) == Name:
			return i
	return -1
