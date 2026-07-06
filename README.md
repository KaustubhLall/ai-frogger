# AI Frogger

A high-performance Frogger-style AI simulation environment and visualizer built in C17 with raylib.

## Overview

AI Frogger is a local AI training sandbox where agents learn to cross a road and river, avoiding cars, riding logs, and reaching the goal. The core is a clean, fast, deterministic simulator with a polished visualizer.

**Key principles:**
- Simulator first, models second
- Deterministic seeded simulation
- No heap allocation in the inner loop
- Clean C ABI for future Python/ML integration
- No dependency on PyTorch, TensorFlow, or any ML framework

## Build

### Prerequisites

- C17 compiler (GCC, Clang, or MSVC)
- CMake 3.16+

### Build instructions

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build everything (including visualizer with raylib)
cmake --build build --config Release

# Build without visualizer (headless only)
cmake -B build -DCMAKE_BUILD_TYPE=Release -DAI_FROGGER_BUILD_VIZ=OFF
cmake --build build --config Release
```

raylib is fetched automatically via CMake FetchContent when not found on the system.

## Run tests

```bash
cd build
ctest --output-on-failure -C Release
```

## Run headless simulation

```bash
# Heuristic agent, 1000 episodes
./build/src/Release/frogger_headless --agent heuristic --episodes 1000 --seed 1337

# Compare all agents
./build/src/Release/frogger_headless --compare --episodes 500

# Save replay
./build/src/Release/frogger_headless --agent heuristic --episodes 1 --seed 42 --replay replay.bin
```

## Run benchmark

```bash
./build/src/Release/frogger_benchmark --episodes 10000 --seed 1
```

## Run visualizer

```bash
# Default: compare random, heuristic, and greedy agents across epochs
./build/src/Release/frogger_viz --seed 1337 --epochs 500

# Specific agents (repeat --agent for multiple)
./build/src/Release/frogger_viz --agent random --agent scripted --agent heuristic --agent greedy

# Replay mode
./build/src/Release/frogger_viz --replay replay.bin
```

### Visualizer views

- **Arena View (1)**: Full board with local observation, status panel, reward graph, action distribution, decision trace, event log, and controls
- **Graphs View (2)**: Per-epoch reward, running average reward, action distribution, and training overview table for all agents
- **Comparison View (3)**: Side-by-side mini arenas for all agents with training overview panel

### Visualizer controls

- **SPACE**: Pause/Resume
- **R**: Reset all sessions
- **+/-**: Speed multiplier
- **S**: Step (when paused)
- **TAB**: Switch active agent
- **1/2/3**: Switch view (Arena / Graphs / Comparison)
- **ESC**: Quit

## Project structure

```
/src
  /core       - RNG, config, math utils, ring buffer, replay, metrics
  /env        - Frogger environment, state, map, rules, danger, observation, reward
  /agents     - Agent interface and implementations (random, scripted, heuristic, greedy)
  /sim        - Runner, benchmark, evaluator
  /viz        - Raylib visualizer (renderer, dashboard, charts, UI controls, session manager)
  /cli        - Headless and benchmark CLI tools
/tests        - CTest test suite
/assets       - Config files
```

## Agents

| Agent | Description |
|-------|-------------|
| `random` | Uniform random valid actions |
| `scripted` | Moves up when possible, avoids obvious danger |
| `heuristic` | Prioritizes forward progress, avoids cars, rushes goal when close |
| `greedy` | Always moves toward goal, ignores danger |

## Environment

The Frogger environment models the classic arcade game:
- **Grid**: 13x14 tiles with goal row, river lanes, road lanes, and start area
- **River**: 6 lanes with moving logs of varying speed, length, and direction
- **Road**: 6 lanes with moving cars of varying speed, length, and direction
- **Frog**: Starts at bottom center, must reach the goal row at top
- **Death**: Hit by car (road), fall in water without log (river), or carried off-screen by log
- **Win**: Reach the goal row

### Actions

| Action | Description |
|--------|-------------|
| `UP` | Move frog up one tile |
| `DOWN` | Move frog down one tile |
| `LEFT` | Move frog left one tile |
| `RIGHT` | Move frog right one tile |
| `WAIT` | Stay in place |

### Reward Components

- **Forward reward**: Small positive for moving toward goal
- **Goal reward**: Large positive for reaching goal
- **Death penalty**: Negative for car collision
- **Drowning penalty**: Negative for falling in water
- **Timeout penalty**: Negative for running out of steps
- **Backward penalty**: Small negative for moving away from goal
- **Stall penalty**: Small negative for waiting
- **Log riding reward**: Small positive for riding a log
- **Invalid action penalty**: Negative for trying to move off-grid
