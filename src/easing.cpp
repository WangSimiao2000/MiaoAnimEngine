#include "animengine/easing.h"

namespace animengine {

float stepStart(const float start, const float end, float t) {
    return (t > 0.0f) ? end : start;
}

float stepEnd(const float start, const float end, float t) {
    return (t >= 1.0f) ? end : start;
}

float linear(const float start, const float end, float t) {
    return start + t * (end - start);
}

float easeIn(const float start, const float end, float t) {
    /* Robert Penner's quadratic ease-in: accelerates from zero velocity */
    t = t * t;
    return start + t * (end - start);
}

float easeOut(const float start, const float end, float t) {
    /* Robert Penner's quadratic ease-out: decelerates to zero velocity */
    t = 1.0f - (1.0f - t) * (1.0f - t);
    return start + t * (end - start);
}

float easeInOut(const float start, const float end, float t) {
    /* Robert Penner's quadratic ease-in-out: accelerates then decelerates */
    if (t < 0.5f) {
        t = 2.0f * t * t;
    } else {
        t = 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
    }
    return start + t * (end - start);
}
}  // namespace animengine
