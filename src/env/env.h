#ifndef FROGGER_ENV_H
#define FROGGER_ENV_H

#include "core/config.h"
#include "core/rng.h"
#include "core/ring_buffer.h"
#include "env/types.h"
#include "env/frogger_state.h"
#include "env/frogger_observation.h"
#include "env/frogger_reward.h"
#include "env/frogger_danger.h"

typedef struct FroggerEnv {
    FroggerState state;
    FroggerConfig config;
    RNG rng;
    DangerMap danger;
    RewardBreakdown last_reward;
    IntRingBuffer action_history;
    Action last_action;
    int prev_frog_y;
    int steps_since_progress;
} FroggerEnv;

void env_init(FroggerEnv* env, const FroggerConfig* config);
void env_reset(FroggerEnv* env, uint64_t seed);
StepResult env_step(FroggerEnv* env, Action action);
void env_observe(const FroggerEnv* env, Observation* obs);

typedef struct {
    FroggerState state;
    DangerMap danger;
    RewardBreakdown last_reward;
    Action last_action;
    char decision_text[256];
    int episode;
    int total_episodes;
    float cumulative_reward;
} DebugSnapshot;

void env_get_debug_snapshot(const FroggerEnv* env, DebugSnapshot* out);

#endif /* FROGGER_ENV_H */
