#include "env/env.h"
#include "core/replay.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    FroggerConfig cfg;
    config_crossing(&cfg);
    cfg.seed = 42;
    cfg.max_steps = 20;

    FroggerEnv env;
    env_init(&env, &cfg);

    Replay* replay = (Replay*)calloc(1, sizeof(Replay));
    replay_init(replay, &cfg, 42);

    for (int i = 0; i < 10; i++) {
        Action a = (i % 3 == 0) ? ACTION_UP : (i % 3 == 1) ? ACTION_DOWN : ACTION_WAIT;
        StepResult r = env_step(&env, a);
        replay_record(replay, a);
        if (r.done) break;
    }

    assert(replay->action_count > 0);
    assert(replay->seed == 42);

    const char* path = "test_replay_frogger.bin";
    assert(replay_save(replay, path) == 1);

    Replay* loaded = (Replay*)calloc(1, sizeof(Replay));
    assert(replay_load(loaded, path) == 1);

    assert(loaded->seed == replay->seed);
    assert(loaded->action_count == replay->action_count);
    for (int i = 0; i < replay->action_count; i++) {
        assert(loaded->actions[i] == replay->actions[i]);
    }

    free(replay);
    free(loaded);
    printf("test_replay: ALL PASSED\n");
    return 0;
}
