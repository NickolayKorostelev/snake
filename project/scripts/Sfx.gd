extends Node

## Autoload that synthesizes short sound effects at runtime (no audio assets needed).

const RATE := 22050

var _players: Dictionary = {}

func _ready() -> void:
	_add("eat", _tone(660.0, 0.09, 0.35, 1760.0))
	_add("gameover", _tone(220.0, 0.55, 0.4, 70.0))
	_add("click", _tone(440.0, 0.04, 0.2, 440.0))
	_add("levelup", _tone(520.0, 0.18, 0.35, 1040.0))

func _add(n: String, stream: AudioStreamWAV) -> void:
	var p := AudioStreamPlayer.new()
	p.stream = stream
	p.bus = "Master"
	add_child(p)
	_players[n] = p

func play(n: String) -> void:
	if _players.has(n):
		var p: AudioStreamPlayer = _players[n]
		p.play()

## Builds a mono 16-bit WAV: a sine sweep from `f0` to `f1` with a linear decay.
func _tone(f0: float, dur: float, vol: float, f1: float) -> AudioStreamWAV:
	var n := int(RATE * dur)
	var data := PackedByteArray()
	data.resize(n * 2)
	var phase := 0.0
	for i in n:
		var t := float(i) / RATE
		var frac := t / dur
		var freq: float = lerpf(f0, f1, frac)
		phase += TAU * freq / RATE
		var env := 1.0 - frac
		var s := sin(phase) * env * vol
		var v := int(clampf(s, -1.0, 1.0) * 32767.0)
		data.encode_s16(i * 2, v)
	var w := AudioStreamWAV.new()
	w.format = AudioStreamWAV.FORMAT_16_BITS
	w.mix_rate = RATE
	w.stereo = false
	w.data = data
	return w