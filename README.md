# AI Frogger

A deterministic Frogger-style simulation environment, baseline-agent suite, and optional raylib visualizer written in C17.

AI Frogger is built around a simple idea: keep the simulator fast, reproducible, and easy to inspect before adding heavier learning systems. The repository includes a headless environment, replay support, benchmark tooling, baseline agents, a genetic-algorithm trainer for a linear neuro policy, metrics export, JSON config loading, tests, and an optional visualizer.

## Highlights

- **Deterministic simulation**: fixed seeds produce repeatable maps, traffic, rewards, and terminal states.
- **Headless-first design**: benchmarks and tests run without raylib or a GPU.
- **Optional visualizer**: raylib UI for inspecting agents, local observations, rewards, action distributions, and comparisons.
- **Flat observations**: `obs_to_flat` exposes a compact vector for future Python, C API, or reinforcement-learning bindings.
- **Baseline policies**: random, scripted, heuristic, and greedy agents for sanity checks and regressions.
- **Neuro agent + GA trainer**: a linear weighted policy agent with a genetic-algorithm trainer (`frogger_train`).
- **Metrics export**: CSV and JSON output with per-terminal-reason breakdowns.
- **JSON config**: load custom configs from `assets/config/default.json` or any JSON file.
- **CI-friendly build**: the headless target can build and test without fetching visualizer dependencies.

## Current scope

This project is an environment and evaluation harness with a lightweight built-in trainer. The built-in agents are intentionally small baselines:

| Agent | Purpose |
| --- | --- |
| `random` | Samples uniformly from valid moves. Useful as a lower bound. |
| `scripted` | Moves forward when the next tick is safe, otherwise dodges or waits. |
| `heuristic` | Prioritizes safe forward progress using one-tick-ahead danger features. |
| `greedy` | Moves toward the goal while ignoring danger, useful as a weak contrast. |
| `neuro` | Linear weighted policy over flat observations. Weights are trained via `frogger_train` or loaded from a file. |

The `neuro` agent is a simple linear model (weight matrix over the flat observation vector). It is not a deep network — it demonstrates the training pipeline and weight save/load, not state-of-the-art RL. The genetic algorithm trainer evolves weights via elitism + Gaussian mutation, which is sufficient for small maps but will not scale to complex policies.

Good next steps would be a NEAT, Q-learning, PPO, or other external trainer that consumes the existing observation vector and writes actions back into the environment.

## Build

### Requirements

- CMake 3.16+
- A C17 compiler: GCC, Clang, or MSVC
- Optional: raylib for the visualizer. If raylib is not installed, CMake can fetch it when `AI_FROGGER_BUILD_VIZ=ON`.

### Headless build

Use this for CI, tests, and benchmarking when you do not need the raylib UI:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DAI_FROGGER_BUILD_VIZ=OFF
cmake --build build --config Release --parallel
ctest --test-dir build --output-on-failure -C Release
```

### Full build with visualizer

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

Executable paths depend on the generator:

- Single-config generators such as Unix Makefiles or Ninja usually output `build/src/frogger_headless`.
- Multi-config generators such as Visual Studio usually output `build/src/Release/frogger_headless.exe`.

## Run

### Headless simulation

```bash
# Heuristic agent, 1000 episodes
./build/src/frogger_headless --agent heuristic --episodes 1000 --seed 1337

# Neuro agent with saved weights
./build/src/frogger_headless --agent neuro --weights best.bin --episodes 100 --seed 42

# Compare all baseline agents
./build/src/frogger_headless --compare --episodes 500 --seed 7

# Save one replay
./build/src/frogger_headless --agent heuristic --episodes 1 --seed 42 --replay replay.bin

# Export metrics to CSV/JSON
./build/src/frogger_headless --agent heuristic --episodes 100 --export-csv metrics.csv --export-json metrics.json

# Load custom config
./build/src/frogger_headless --agent heuristic --config assets/config/default.json --episodes 100
```

On Windows with Visual Studio, use the `build/src/Release/` path instead:

```powershell
.\build\src\Release\frogger_headless.exe --agent heuristic --episodes 1000 --seed 1337
```

### Benchmark

```bash
./build/src/frogger_benchmark --episodes 10000 --seed 1
```

### Genetic algorithm trainer

```bash
# Train a neuro agent for 50 generations
./build/src/frogger_train --population 20 --generations 50 --episodes 10 --seed 1337 --save best.bin

# Evaluate saved weights
./build/src/frogger_train --load best.bin --eval 100 --seed 42

# Train with custom config
./build/src/frogger_train --config assets/config/default.json --generations 30 --save best.bin
```

Weight files use a binary format with magic bytes (`FNRO`), version, and dimension metadata (action count, weight size, observation flat size) to detect mismatches.

### Visualizer

```bash
# Default comparison view
./build/src/frogger_viz --seed 1337 --epochs 500

# Select specific agents
./build/src/frogger_viz --agent random --agent scripted --agent heuristic --agent greedy

# Replay a saved run
./build/src/frogger_viz --replay replay.bin
```

Visualizer controls:

| Key | Action |
| --- | --- |
| `Space` | Pause or resume |
| `R` | Reset sessions |
| `+` / `-` | Change speed multiplier |
| `S` | Step while paused |
| `Tab` | Switch active agent |
| `1` / `2` / `3` | Arena, graphs, comparison views |
| `Esc` | Quit |

## Environment model

The default crossing map is a 13 x 14 grid:

- goal row at the top
- river lanes with moving logs
- median strip
- road lanes with moving cars
- start row at the bottom

Each step applies an action, advances dynamic objects, resolves log riding/collisions, computes rewards, and checks terminal state. Observation danger flags are computed one tick ahead, so baseline agents can avoid moves that become unsafe after cars or logs move.

### Actions

| Action | Description |
| --- | --- |
| `UP` | Move one tile toward the goal |
| `DOWN` | Move one tile away from the goal |
| `LEFT` | Move one tile left |
| `RIGHT` | Move one tile right |
| `WAIT` | Stay in place for one tick |

### Reward components

- forward progress reward
- goal reward
- car collision penalty
- water/off-map penalty
- timeout penalty
- backward movement penalty
- stall penalty
- log-riding reward
- invalid action penalty

## Project layout

```text
src/
  agents/     baseline policies + neuro agent
  cli/        headless, benchmark, and trainer executables
  core/       config, RNG, metrics, replay, utility code
  env/        map, state transition, rules, danger, observations, rewards
  sim/        runners, evaluation, benchmarks
  train/      genetic algorithm trainer
  viz/        optional raylib visualizer
tests/        CTest-based regression suite
assets/       default config assets
docs/         design and architecture notes
```

## Development workflow

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DAI_FROGGER_BUILD_VIZ=OFF
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

For behavior changes, add or update tests that pin the intended transition order, terminal reason, observation feature, or agent decision. `docs/architecture.md` contains the current simulation contract and recommended smoke checks.

## Roadmap ideas

- C ABI or Python binding for external trainers
- NEAT or deeper network policy
- tabular/Q-learning baseline for small maps
- replay viewer improvements and replay metadata
- richer benchmark output and plotting
- config-file presets for different map layouts
