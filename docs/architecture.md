# Architecture

This document describes the system as it is implemented today in `apps/desktop/`. The planned features are described in the [README](../README.md).

## Overview

Atelier3D is a desktop application built on a custom C++20 engine. The code is one library and one program, the editor. Games are folders of data that the editor creates and plays at runtime:

```text
┌───────────────────────┐
│   atelier_engine      │  static library (AtelierEngine)
│   core · platform ·   │
│   renderer · scene ·  │
│   game · ui · input · │
│   assets · physics    │
└──────────┬────────────┘
           │ links
┌──────────┴────────────┐
│   atelier_editor      │  AtelierEditor: creates, picks and plays games
└──────────┬────────────┘
           │ creates / reads
┌──────────┴────────────┐
│   game projects       │  <folder of AtelierEditor>/projects/<game>/
│   (data, no code)     │  configs/game.cfg, data/scenes/*.scene, assets/
└───────────────────────┘
```

## Components

### Engine (`projects/atelier_engine`)

Public headers are in `include/engine/<module>/`, and each module's sources are in `src/<module>/`.

| Module | Responsibility |
|---|---|
| `core` | `Application` (main loop: `OnInit`, `OnUpdate`, `OnShutdown`), logging, timing |
| `platform` | `Window` interface with a Win32 backend on Windows, a GLFW + X11 backend on Linux, and a `NullWindow` for headless use |
| `renderer` | OpenGL drawing (fixed-function OpenGL 1.x) |
| `scene` | Entities and the scene container |
| `game` | `GameProject` (load, find and create games) and `GameRuntime` (play a game's scene) |
| `ui` | Dear ImGui integration for the editor |
| `input`, `assets`, `physics` | Input state, asset path lookup, physics world (early stage) |

### Editor (`projects/atelier_editor`)

`AtelierEditor` is an engine `Application` with ImGui panels (scene view, game view, hierarchy, inspector, console). Its *Game Project* window lists the games in the `projects` folder next to the editor, plays the selected one in place (F5), and creates new games there. `AtelierEditor --new-game <name> [--template empty|cube|pyramid]` creates a game without opening a window.

## Game projects

A folder is a game project when it contains `configs/game.cfg`. Because games are data, adding one needs no C++ code or build change.

```text
<game>/
├── configs/game.cfg        name, window_width, window_height, start_scene
├── data/scenes/*.scene     one object per line: <cube|pyramid> [x=..] [y=..] [z=..] [rotation_speed=..]
├── assets/                 models, textures, materials, shaders, audio
└── scripts/
```

`GameProject::ProjectsRoot()` is the `projects` folder inside the folder containing the running executable (`Platform::ExecutableDir() / "projects"`), so games are created and found next to `AtelierEditor`, wherever it was launched from. The folder is created with the first game. During development that is inside `apps/desktop/build/`, which is not committed and is cleared by `clean.bat` and CI builds. `GameProject::Create()` makes the folder layout above from a template, and it validates the name so a game can't be created outside that folder.

## Technology stack

| Concern | Choice |
|---|---|
| Language | C++20 |
| Build | CMake 3.20+ (Visual Studio generator on Windows, Ninja + GCC on Linux, MinGW-w64 for Windows cross-builds) |
| Windowing | Win32 (Windows), GLFW 3.4 + X11 (Linux, fetched by CMake) |
| Rendering | OpenGL |
| Editor UI | Dear ImGui |
| Third-party code | Committed in `apps/desktop/libraries/` (GLEW, GLFW, glm, ImGui, ImGuizmo, json, Lua, sol2, stb, FMOD, gRPC, ...) |
| Tests | GoogleTest (fetched by CMake), in `tests/unit/` |
| Checks | clang-format, clang-tidy, cppcheck, compiler warnings (`-Wall -Wextra`) |
| CI | Jenkins (`Jenkinsfile`): Linux and Windows builds, tests, lint, static analysis, packaging |
