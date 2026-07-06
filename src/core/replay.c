#include "core/replay.h"
#include <string.h>
#include <stdio.h>

void replay_init(Replay* r, const FroggerConfig* cfg, uint64_t seed) {
    memset(r, 0, sizeof(Replay));
    r->config = *cfg;
    r->seed = seed;
}

void replay_record(Replay* r, Action action) {
    if (r->action_count < MAX_REPLAY_ACTIONS) {
        r->actions[r->action_count++] = action;
    }
}

int replay_save(const Replay* r, const char* path) {
    FILE* f = fopen(path, "wb");
    if (!f) return 0;
    fwrite(&r->config, sizeof(FroggerConfig), 1, f);
    fwrite(&r->seed, sizeof(uint64_t), 1, f);
    fwrite(&r->action_count, sizeof(int), 1, f);
    fwrite(r->actions, sizeof(Action), r->action_count, f);
    fclose(f);
    return 1;
}

int replay_load(Replay* r, const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return 0;
    fread(&r->config, sizeof(FroggerConfig), 1, f);
    fread(&r->seed, sizeof(uint64_t), 1, f);
    fread(&r->action_count, sizeof(int), 1, f);
    if (r->action_count > MAX_REPLAY_ACTIONS) r->action_count = MAX_REPLAY_ACTIONS;
    fread(r->actions, sizeof(Action), r->action_count, f);
    fclose(f);
    return 1;
}
