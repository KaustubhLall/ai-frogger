#ifndef FROGGER_TYPES_H
#define FROGGER_TYPES_H

typedef enum {
    ACTION_UP = 0,
    ACTION_DOWN = 1,
    ACTION_LEFT = 2,
    ACTION_RIGHT = 3,
    ACTION_WAIT = 4,
    ACTION_COUNT = 5
} Action;

typedef enum {
    TERMINAL_NONE = 0,
    TERMINAL_GOAL,
    TERMINAL_DEATH_CAR,
    TERMINAL_DEATH_WATER,
    TERMINAL_TIMEOUT,
    TERMINAL_OFF_MAP
} TerminalReason;

typedef struct {
    float reward;
    int done;
    TerminalReason terminal_reason;
} StepResult;

#endif /* FROGGER_TYPES_H */
