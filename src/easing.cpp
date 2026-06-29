#include "animengine/easing.h"

namespace animengine {

float stepStart(float start, float end, float t) {
    return (t > 0.0f) ? end : start;
}

float stepEnd(float start, float end, float t) {
    return (t >= 1.0f) ? end : start;
}

float linear(float start, float end, float t) {
    return start + t * (end - start);
}

float easeIn(float start, float end, float t) {
    /* Robert Penner's quadratic ease-in: accelerates from zero velocity */
    t = t * t;
    return start + t * (end - start);
}

float easeOut(float start, float end, float t) {
    /* Robert Penner's quadratic ease-out: decelerates to zero velocity */
    t = 1.0f - (1.0f - t) * (1.0f - t);
    return start + t * (end - start);
}

float easeInOut(float start, float end, float t) {
    /* Robert Penner's quadratic ease-in-out: accelerates then decelerates */
    if (t < 0.5f) {
        t = 2.0f * t * t;
    } else {
        t = 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
    }
    return start + t * (end - start);
}
}  // namespace animengine
