extends Node

## Autoload singleton that persists the high score to user://highscore.save

const SAVE_PATH := "user://highscore.save"

var score: int = 0

func _ready() -> void:
	_load()

func submit(new_score: int) -> void:
	if new_score > score:
		score = new_score
		_save()

func _load() -> void:
	if not FileAccess.file_exists(SAVE_PATH):
		return
	var f := FileAccess.open(SAVE_PATH, FileAccess.READ)
	if f == null:
		return
	score = f.get_32()

func _save() -> void:
	var f := FileAccess.open(SAVE_PATH, FileAccess.WRITE)
	if f == null:
		return
	f.store_32(score)