#ifndef FROGGER_RULES_H
#define FROGGER_RULES_H

#include "env/frogger_state.h"
#include "env/types.h"
#include "core/config.h"

int rules_try_move(FroggerState* state, Action action, const FroggerConfig* cfg);
int rules_check_collision(const FroggerState* state);
int rules_check_drowning(const FroggerState* state);
TerminalReason rules_check_terminal(const FroggerState* state, const FroggerConfig* cfg);

#endif /* FROGGER_RULES_H */
