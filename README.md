# Snake Game

Snake game built with a **pure C++ core** (fully unit-tested) and a **Godot 4 GDExtension** layer for rendering, input, and audio. No game logic depends on Godot.

## Layout

```
snake-game/
├── src/
│   ├── core/      # Pure C++ library (no Godot deps) — snake_core
│   ├── gdext/     # GDExtension wrapper (SnakeGame node) — snake_gdext
│   └── tests/     # GoogleTest unit tests
├── project/       # Godot project (scenes, scripts, export preset)
├── godot-cpp/     # Submodule (Godot C++ bindings)
├── CMakeLists.txt
└── CMakePresets.json
```

## Build

Requires: VS 2022 Build Tools (MSVC), CMake 3.25+, Python 3, Git, Godot 4.7+.

```powershell
git clone --recursive https://github.com/NickolayKorostelev/snake.git
cd snake

cmake --preset windows-msvc          # configure (debug)
cmake --build --preset windows-msvc-debug

# tests
ctest --preset windows-msvc-debug
```

Release (for export):
```powershell
cmake --preset windows-msvc-release
cmake --build --preset windows-msvc-release
```

## Run

Open `project/` in Godot 4.7+, or run the exported build:

```
dist/snake_game.exe
```

Controls: **WASD/Arrows** move, **1/2/3** difficulty, **Esc** pause, **Enter** restart.

## Export (Windows)

```powershell
godot --headless --path project --export-release "Windows Desktop" dist/snake_game.exe
```

Output: `dist/snake_game.exe` + `snake_gdext.windows.template_release.x86_64.dll` (keep the DLL next to the exe).

## Features

- Pure C++ core with fixed-timestep game loop
- Wall wrapping / hard walls, self-collision, food, growth, progressive speedup
- Difficulty presets (Easy / Normal / Hard)
- Score + persistent high score (`user://highscore.save`)
- Pause, game-over screen, restart
- Procedural sound effects (no audio assets)
- Eat particle burst
- Keyboard + on-screen touch controls