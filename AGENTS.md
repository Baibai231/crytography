# AGENTS.md

## Project Overview
Qt6/C++ desktop cryptography game. Entry: `main.cpp` creates `StartScreen` and `GameWindow`, connected via `startClicked` signal.

## Build System
- Qt project: `ctytography.pro`
- Build output: `build/Desktop_Qt_6_10_1_MinGW_64_bit-Debug/`
- Build commands: `qmake && make` or use Qt Creator
- Requires: `c++17`, Qt multimedia modules

## Core Modules
| File | Purpose |
|------|---------|
| `GameWindow.*` | QGraphicsView-based main view; owns LevelManager, PuzzleManager, UIManager, VideoController |
| `startscreen.*` | Start menu widget with Q_OBJECT |
| `Player.*` | Animated player entity (12 run frames in images/) |
| `LevelManager.*` | Level loading and progression |
| `PuzzleManager.*` | Puzzle orchestration (Caesar, Rail Fence, etc.) |
| `aimanager.*` / `aichatdialog.*` | AI chat integration |
| `caesarpuzzle.*` / `railfencepuzzle.*` / `railfencevisualizer.*` | Cipher puzzles |
| `uimanager.*` | UI overlay management |
| `videocontroller.*` | Video playback |
| `inputcontroller.*` | Keyboard input handling |
| `gamestate.*` | Game state persistence |

## Resources
- Qt resource file: `res.qrc` (prefix `/`)
- Images: `images/` (maya_bg.png, run_*.png, arrow.png, stone.png)

## Qt Conventions
- Signals/slots: classes must inherit `QObject` and include `Q_OBJECT` macro
- Headers use forward declarations to minimize includes
- `CONFIG += console` for debug output visibility

## Build Artifacts (gitignore)
- `build/`, `*.pro.user*`, `moc_*.cpp`, `qrc_*.cpp`, `ui_*.h`
- Generated: `Makefile*`, `*.o`, `*.exe`, `.rcc/`, `.moc/`
