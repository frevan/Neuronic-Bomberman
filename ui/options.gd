extends TScene


signal OnLeave


func BeforeShow() -> void:
	super()
	$FXVolumeSlider.set_value_no_signal(Settings.FXVolume * $FXVolumeSlider.max_value)

func AfterHide() -> void:
	super()


func _on_leave_button_pressed() -> void:
	OnLeave.emit()


func _on_fx_volume_slider_drag_ended(value_changed: bool) -> void:
	if value_changed:
		Settings.FXVolume = $FXVolumeSlider.value
