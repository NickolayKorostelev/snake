extends Node2D

## Orchestrates presentation: score, high score, difficulty, pause, game-over UI,
## sound and particles. All game logic lives in the C++ SnakeGame node.

const CELL_SIZE := 15  # keep in sync with src/core/Config.hpp

@onready var snake_game = $SnakeGame
@onready var score_label: Label = $UI/Score
@onready var difficulty_label: Label = $UI/Difficulty
@onready var game_over_label: Label = $UI/GameOver
@onready var pause_label: Label = $UI/Pause
@onready var eat_burst = $EatBurst
@onready var touch_layer = $Touch

const DIFFICULTY_NAMES := ["Easy", "Normal", "Hard"]

func _ready() -> void:
	# Keep processing input while the tree is paused so we can unpause.
	process_mode = Node.PROCESS_MODE_ALWAYS
	# The snake itself must stop while paused.
	snake_game.process_mode = Node.PROCESS_MODE_PAUSABLE

	_bind_keys()
	_bind_touch()
	_make_burst_texture()

	snake_game.food_eaten.connect(_on_food_eaten)
	snake_game.game_over.connect(_on_game_over)

	touch_layer.visible = DisplayServer.is_touchscreen_available()

	game_over_label.visible = false
	pause_label.visible = false
	_apply_difficulty(1)
	_update_score(0)

func _bind_keys() -> void:
	# Augment the built-in ui_* actions with WASD so both schemes work.
	_add_keys("ui_up", [KEY_W])
	_add_keys("ui_down", [KEY_S])
	_add_keys("ui_left", [KEY_A])
	_add_keys("ui_right", [KEY_D])

func _add_keys(action: String, keys: Array) -> void:
	if not InputMap.has_action(action):
		InputMap.add_action(action)
	for k in keys:
		var ev := InputEventKey.new()
		ev.physical_keycode = k
		InputMap.action_add_event(action, ev)

func _bind_touch() -> void:
	_bind_touch_button($Touch/Up, "ui_up")
	_bind_touch_button($Touch/Down, "ui_down")
	_bind_touch_button($Touch/Left, "ui_left")
	_bind_touch_button($Touch/Right, "ui_right")

func _bind_touch_button(btn: Button, action: String) -> void:
	btn.focus_mode = Control.FOCUS_NONE
	btn.button_down.connect(_on_touch_down.bind(action))
	btn.button_up.connect(_on_touch_up.bind(action))

func _on_touch_down(action: String) -> void:
	Input.action_press(action)

func _on_touch_up(action: String) -> void:
	Input.action_release(action)

func _make_burst_texture() -> void:
	if eat_burst.texture == null:
		var img := Image.create(8, 8, false, Image.FORMAT_RGBA8)
		img.fill(Color(1, 1, 1, 1))
		eat_burst.texture = ImageTexture.create_from_image(img)

func _unhandled_input(event: InputEvent) -> void:
	if event.is_action_pressed("ui_cancel"):
		_set_paused(not get_tree().paused)
		return
	if event is InputEventKey and event.pressed and not event.echo:
		match event.physical_keycode:
			KEY_1: _apply_difficulty(0)
			KEY_2: _apply_difficulty(1)
			KEY_3: _apply_difficulty(2)
	if event.is_action_pressed("ui_accept") and snake_game.is_game_over():
		get_tree().reload_current_scene()

func _apply_difficulty(index: int) -> void:
	snake_game.set_difficulty(index)
	difficulty_label.text = "%s  [1/2/3]" % DIFFICULTY_NAMES[index]
	Sfx.play("click")

func _set_paused(p: bool) -> void:
	get_tree().paused = p
	pause_label.visible = p
	Sfx.play("click")

func _on_food_eaten(new_score: int) -> void:
	_update_score(new_score)
	HighScore.submit(new_score)
	Sfx.play("eat")
	if new_score % 5 == 0:
		Sfx.play("levelup")
	var cell = snake_game.get_last_eaten_cell()
	if cell.x >= 0:
		eat_burst.position = Vector2(cell.x * CELL_SIZE + CELL_SIZE / 2.0, cell.y * CELL_SIZE + CELL_SIZE / 2.0)
		eat_burst.restart()

func _on_game_over(final_score: int) -> void:
	HighScore.submit(final_score)
	Sfx.play("gameover")
	game_over_label.text = "GAME OVER\nScore: %d\nHigh Score: %d\n\nPress Enter to restart" % [final_score, HighScore.score]
	game_over_label.visible = true
	_set_paused(false)

func _update_score(s: int) -> void:
	score_label.text = "Score: %d    High: %d" % [s, HighScore.score]