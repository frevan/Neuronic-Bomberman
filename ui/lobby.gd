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
	Client.OnWinConditionChanged.connect(_client_win_condition_changed)
	Client.OnMaxTimeChanged.connect(_client_max_time_changed)
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
	Client.OnWinConditionChanged.disconnect(_client_win_condition_changed)
	Client.OnMaxTimeChanged.disconnect(_client_max_time_changed)
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

func _client_win_condition_changed(_Condition: Constants.WinCondition, _Value: int) -> void:
	_AdjustWinConditionControls()
	pass

func _client_max_time_changed(_Value: int) -> void:
	_AdjustMaxTimeSelector()
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
	$%WinConditionSelector.visible = Network.IsServer()
	$%WinConditionLabel.visible = !Network.IsServer()
	$%TimeSelector.visible = Network.IsServer()
	_AdjustWinConditionControls()
	pass

func _AdjustWinConditionControls() -> void:
	var v: int = 0
	var idx: int = 0
	var s: String
	match Client.Data.WinCondition:
		Constants.WinCondition.NUM_ROUNDS:
			idx = 0
			v = Client.Data.NumRounds
			s = "Rounds"
		Constants.WinCondition.SCORE:
			idx = 1
			v = Client.Data.ScoreToWin
			s = "Score"
	if Network.IsServer():
		$%WinConditionSelector.select(idx)
		$%WinConditionSpin.set_value_no_signal(v)
	else:
		$%WinConditionLabel.text = s + ": " + str(v)
	pass


func _AdjustMaxTimeSelector() -> void:
	var idx: int = 10
	match Client.Data.MaxTime:
		30: idx = 0
		60: idx = 1
		2*60: idx = 2
		3*60: idx = 3
		4*60: idx = 4
		5*60: idx = 5
		6*60: idx = 6
		7*60+30: idx = 7
		10*60: idx = 8
		15*60: idx = 9
		0: idx = 10
	if Network.IsServer():
		$%TimeSelector.select(idx)
	else:
		$%TimeSelectorLabel.text = "Time: " + $%TimeSelector.get_item_text(idx)
	pass


func _IndexOfMapInList(Name: String) -> int:
	for i in $MapsList.item_count:
		if $MapsList.get_item_text(i) == Name:
			return i
	return -1


func _on_win_condition_selector_item_selected(index: int) -> void:
	var condition: Constants.WinCondition
	var v: int
	match index:
		0: 
			condition = Constants.WinCondition.NUM_ROUNDS
			v = Client.Data.NumRounds
		1: 
			condition = Constants.WinCondition.SCORE
			v = Client.Data.ScoreToWin
	Client.RequestWinCondition(condition, v)
	pass


func _on_win_condition_spin_value_changed(value: float) -> void:
	var v: int = floor(value)
	Client.RequestWinCondition(Client.Data.WinCondition, v)
	pass


func _on_time_selector_item_selected(index: int) -> void:
	var seconds: int = 0
	match index:
		0: seconds = 30
		1: seconds = 60
		2: seconds = 2*60
		3: seconds = 3*60
		4: seconds = 4*60
		5: seconds = 5*60
		6: seconds = 6*60
		7: seconds = 7*60+30
		8: seconds = 10*60
		9: seconds = 15*60
		10: seconds = 0 # infinite
	Client.RequestMaxTime(seconds)
	pass
