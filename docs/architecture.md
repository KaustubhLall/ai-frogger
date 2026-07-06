# Architecture Notes

AI Frogger is structured as a small deterministic simulation engine with optional presentation and evaluation layers. The goal is to make the environment easy to benchmark, inspect, and eventually bind to external learning code without entangling the simulator with a specific ML framework.

## Core loop

Each step follows the same order:

1. Apply the selected action if it stays within the map.
2. Advance logs and cars by one simulation tick.
3. Resolve log riding or river failure for the frog.
4. Compute reward components.
5. Resolve terminal state.
6. Recompute danger maps for visualization and debugging.

Keeping this order explicit matters because an action that looks safe before traffic moves may be unsafe after the tick update. Observation danger flags therefore look one tick ahead for cars and logs.

## Layers

| Layer | Responsibility |
| --- | --- |
| `src/core` | Config, RNG, replay, ring buffer, metrics, and shared utility code. |
| `src/env` | Map generation, state transitions, collision/risk checks, observations, rewards, and debug snapshots. |
| `src/agents` | Baseline policies used to sanity-check the environment. These are not learning agents. |
| `src/sim` | Episode runners, agent comparisons, and benchmark execution. |
| `src/cli` | Headless executables for repeatable runs and smoke tests. |
| `src/viz` | Optional raylib UI for stepping through agents, charts, and comparisons. |
| `tests` | Determinism, environment, observation, reward, replay, and agent behavior coverage. |

## Determinism contract

For a fixed config, seed, and action sequence, the simulator should produce the same rewards, terminal reasons, and state transitions. New features should preserve this unless they intentionally change the model. Tests should prefer synthetic state setup when checking edge cases so they are not brittle against spawn randomness.

## Agent contract

Current built-in agents are baselines:

- `random`: samples uniformly from valid moves.
- `scripted`: moves forward when the next tick is safe, otherwise dodges laterally or waits.
- `heuristic`: prioritizes safe forward progress and uses danger flags to avoid obvious next-tick failures.
- `greedy`: intentionally ignores danger and moves toward the goal as a weak baseline.

Future learning code should use `Observation` and `obs_to_flat` rather than reaching into `FroggerState` directly.

## What to test before changing behavior

Run the headless suite first:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DAI_FROGGER_BUILD_VIZ=OFF
cmake --build build --config Release --parallel
ctest --test-dir build --output-on-failure -C Release
```

Useful smoke checks:

```bash
./build/src/frogger_headless --compare --episodes 25 --seed 7
./build/src/frogger_benchmark --episodes 1000 --seed 1
```

On Windows multi-config generators, the executables may live under `build/src/Release/` instead of `build/src/`.
