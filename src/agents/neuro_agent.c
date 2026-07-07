#include "agents/neuro_agent.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

static float gauss(RNG* rng) {
    float u1 = rng_float(rng);
    float u2 = rng_float(rng);
    if (u1 < 1e-6f) u1 = 1e-6f;
    return sqrtf(-2.0f * logf(u1)) * cosf(2.0f * 3.14159265f * u2);
}

void neuro_weights_init(NeuroWeights* w, RNG* rng) {
    memset(w, 0, sizeof(NeuroWeights));
    for (int a = 0; a < ACTION_COUNT; a++) {
        for (int i = 0; i < NEURO_WEIGHT_SIZE; i++) {
            w->weights[a][i] = gauss(rng) * 0.1f;
        }
    }
    w->obs_size = 0;
}

void neuro_weights_mutate(NeuroWeights* w, const NeuroWeights* src, RNG* rng, float rate) {
    memcpy(w, src, sizeof(NeuroWeights));
    for (int a = 0; a < ACTION_COUNT; a++) {
        for (int i = 0; i < NEURO_WEIGHT_SIZE; i++) {
            if (rng_float(rng) < rate) {
                w->weights[a][i] += gauss(rng) * 0.15f;
            }
        }
    }
}

Action neuro_act(NeuroWeights* w, const Observation* obs, char* decision_text, int decision_text_size) {
    float flat[NEURO_WEIGHT_SIZE];
    int obs_size;
    obs_to_flat(obs, flat, &obs_size);
    w->obs_size = obs_size;

    float scores[ACTION_COUNT];
    for (int a = 0; a < ACTION_COUNT; a++) {
        float dot = 0.0f;
        for (int i = 0; i < obs_size; i++) {
            dot += w->weights[a][i] * flat[i];
        }
        scores[a] = dot;
    }

    Action best = ACTION_WAIT;
    float best_score = -1e30f;
    for (int a = 0; a < ACTION_COUNT; a++) {
        if (obs->valid_actions[a] && scores[a] > best_score) {
            best_score = scores[a];
            best = (Action)a;
        }
    }

    const char* names[] = {"UP", "DOWN", "LEFT", "RIGHT", "WAIT"};
    snprintf(decision_text, decision_text_size, "neuro: %s (%.2f)", names[best], best_score);
    return best;
}

int neuro_weights_save(const NeuroWeights* w, const char* path) {
    FILE* f = fopen(path, "wb");
    if (!f) return 0;

    unsigned char magic[4] = { 'F', 'N', 'R', 'O' };
    int version = 2;
    int action_count = ACTION_COUNT;
    int weight_size = NEURO_WEIGHT_SIZE;
    int obs_flat_size = OBS_FLAT_SIZE;
    int ok = 1;
    ok &= (fwrite(magic, 4, 1, f) == 1);
    ok &= (fwrite(&version, sizeof(int), 1, f) == 1);
    ok &= (fwrite(&action_count, sizeof(int), 1, f) == 1);
    ok &= (fwrite(&weight_size, sizeof(int), 1, f) == 1);
    ok &= (fwrite(&obs_flat_size, sizeof(int), 1, f) == 1);
    ok &= (fwrite(w->weights, sizeof(float), ACTION_COUNT * NEURO_WEIGHT_SIZE, f) == ACTION_COUNT * NEURO_WEIGHT_SIZE);

    fclose(f);
    return ok;
}

int neuro_weights_load(NeuroWeights* w, const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return 0;

    unsigned char magic[4];
    int version;
    if (fread(magic, 4, 1, f) != 1) { fclose(f); return 0; }
    if (magic[0] != 'F' || magic[1] != 'N' || magic[2] != 'R' || magic[3] != 'O') {
        fclose(f);
        return 0;
    }
    if (fread(&version, sizeof(int), 1, f) != 1) { fclose(f); return 0; }

    if (version == 1) {
        /* v1: no dimension metadata, just raw weights */
        if (fread(w->weights, sizeof(float), ACTION_COUNT * NEURO_WEIGHT_SIZE, f) != ACTION_COUNT * NEURO_WEIGHT_SIZE) {
            fclose(f);
            return 0;
        }
        fclose(f);
        return 1;
    }

    if (version != 2) { fclose(f); return 0; }

    int action_count, weight_size, obs_flat_size;
    if (fread(&action_count, sizeof(int), 1, f) != 1) { fclose(f); return 0; }
    if (fread(&weight_size, sizeof(int), 1, f) != 1) { fclose(f); return 0; }
    if (fread(&obs_flat_size, sizeof(int), 1, f) != 1) { fclose(f); return 0; }

    if (action_count != ACTION_COUNT || weight_size != NEURO_WEIGHT_SIZE || obs_flat_size != OBS_FLAT_SIZE) {
        fprintf(stderr, "Weight file dimension mismatch: file has action_count=%d weight_size=%d obs_flat_size=%d, "
                "expected action_count=%d weight_size=%d obs_flat_size=%d\n",
                action_count, weight_size, obs_flat_size,
                ACTION_COUNT, NEURO_WEIGHT_SIZE, OBS_FLAT_SIZE);
        fclose(f);
        return 0;
    }

    if (fread(w->weights, sizeof(float), ACTION_COUNT * NEURO_WEIGHT_SIZE, f) != ACTION_COUNT * NEURO_WEIGHT_SIZE) {
        fclose(f);
        return 0;
    }

    fclose(f);
    return 1;
}
