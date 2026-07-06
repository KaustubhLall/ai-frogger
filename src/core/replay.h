#ifndef FROGGER_REPLAY_H
#define FROGGER_REPLAY_H

#include "core/config.h"
#include "env/frogger_state.h"
#include "env/types.h"

#define MAX_REPLAY_ACTIONS MAX_REPLAY_STEPS

typedef struct {
    FroggerConfig config;
    uint64_t seed;
    Action actions[MAX_REPLAY_ACTIONS];
    int action_count;
} Replay;

void replay_init(Replay* r, const FroggerConfig* cfg, uint64_t seed);
void replay_record(Replay* r, Action action);
int replay_save(const Replay* r, const char* path);
int replay_load(Replay* r, const char* path);

#endif /* FROGGER_REPLAY_H */
