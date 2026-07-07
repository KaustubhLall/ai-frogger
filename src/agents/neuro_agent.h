#ifndef FROGGER_NEURO_AGENT_H
#define FROGGER_NEURO_AGENT_H

#include "env/env.h"
#include "env/frogger_observation.h"
#include "env/types.h"
#include "core/rng.h"

#define NEURO_WEIGHT_SIZE OBS_FLAT_SIZE

typedef struct {
    float weights[ACTION_COUNT][NEURO_WEIGHT_SIZE];
    int obs_size;
} NeuroWeights;

void neuro_weights_init(NeuroWeights* w, RNG* rng);
void neuro_weights_mutate(NeuroWeights* w, const NeuroWeights* src, RNG* rng, float rate);
Action neuro_act(NeuroWeights* w, const Observation* obs, char* decision_text, int decision_text_size);
int neuro_weights_save(const NeuroWeights* w, const char* path);
int neuro_weights_load(NeuroWeights* w, const char* path);

#endif /* FROGGER_NEURO_AGENT_H */
