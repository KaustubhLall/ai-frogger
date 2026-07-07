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

    unsigned char magic[4] = { REPLAY_MAGIC0, REPLAY_MAGIC1, REPLAY_MAGIC2, REPLAY_MAGIC3 };
    int version = REPLAY_VERSION;

    int ok = 1;
    ok &= (fwrite(magic, sizeof(unsigned char), 4, f) == 4);
    ok &= (fwrite(&version, sizeof(int), 1, f) == 1);
    ok &= (fwrite(&r->config, sizeof(FroggerConfig), 1, f) == 1);
    ok &= (fwrite(&r->seed, sizeof(uint64_t), 1, f) == 1);
    ok &= (fwrite(&r->action_count, sizeof(int), 1, f) == 1);
    if (r->action_count > 0) {
        ok &= (fwrite(r->actions, sizeof(Action), (size_t)r->action_count, f) == (size_t)r->action_count);
    }

    fclose(f);
    return ok;
}

int replay_load(Replay* r, const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return 0;

    unsigned char magic[4];
    int version;

    if (fread(magic, sizeof(unsigned char), 4, f) != 4) { fclose(f); return 0; }
    if (magic[0] != REPLAY_MAGIC0 || magic[1] != REPLAY_MAGIC1 ||
        magic[2] != REPLAY_MAGIC2 || magic[3] != REPLAY_MAGIC3) {
        fprintf(stderr, "replay_load: invalid magic bytes\n");
        fclose(f);
        return 0;
    }

    if (fread(&version, sizeof(int), 1, f) != 1) { fclose(f); return 0; }
    if (version != REPLAY_VERSION) {
        fprintf(stderr, "replay_load: unsupported version %d (expected %d)\n", version, REPLAY_VERSION);
        fclose(f);
        return 0;
    }

    if (fread(&r->config, sizeof(FroggerConfig), 1, f) != 1) { fclose(f); return 0; }
    if (fread(&r->seed, sizeof(uint64_t), 1, f) != 1) { fclose(f); return 0; }
    if (fread(&r->action_count, sizeof(int), 1, f) != 1) { fclose(f); return 0; }

    if (r->action_count < 0 || r->action_count > MAX_REPLAY_ACTIONS) {
        fprintf(stderr, "replay_load: action count %d out of range [0, %d]\n",
                r->action_count, MAX_REPLAY_ACTIONS);
        r->action_count = 0;
        fclose(f);
        return 0;
    }

    if (r->action_count > 0) {
        if (fread(r->actions, sizeof(Action), (size_t)r->action_count, f) != (size_t)r->action_count) {
            fclose(f);
            return 0;
        }
    }

    fclose(f);
    return 1;
}
