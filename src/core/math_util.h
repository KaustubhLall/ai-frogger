#ifndef FROGGER_MATH_UTIL_H
#define FROGGER_MATH_UTIL_H

static inline int clampi(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static inline float clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static inline int absi(int v) {
    return v < 0 ? -v : v;
}

static inline int mini(int a, int b) {
    return a < b ? a : b;
}

static inline int maxi(int a, int b) {
    return a > b ? a : b;
}

static inline float absf(float v) {
    return v < 0.0f ? -v : v;
}

static inline float lerpf(float a, float b, float t) {
    return a + (b - a) * t;
}

#endif /* FROGGER_MATH_UTIL_H */
