#include "animengine/easing.h"

namespace animengine {

float easeShape(const Easing easing, const float t) {
    switch (easing) {
        case Easing::StepStart:
            return t > 0.0f ? 1.0f : 0.0f;
        case Easing::StepEnd:
            return t >= 1.0f ? 1.0f : 0.0f;
        case Easing::Linear:
            return t;
        case Easing::EaseIn:
            return t * t;
        case Easing::EaseOut:
            return 1.0f - (1.0f - t) * (1.0f - t);
        case Easing::EaseInOut:
            return t < 0.5f ? 2.0f * t * t : 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
    }
    return t;
}

float stepStart(const float start, const float end, float t) {
    return start + easeShape(Easing::StepStart, t) * (end - start);
}

float stepEnd(const float start, const float end, float t) {
    return start + easeShape(Easing::StepEnd, t) * (end - start);
}

float linear(const float start, const float end, float t) {
    return start + easeShape(Easing::Linear, t) * (end - start);
}

float easeIn(const float start, const float end, float t) {
    return start + easeShape(Easing::EaseIn, t) * (end - start);
}

float easeOut(const float start, const float end, float t) {
    return start + easeShape(Easing::EaseOut, t) * (end - start);
}

float easeInOut(const float start, const float end, float t) {
    return start + easeShape(Easing::EaseInOut, t) * (end - start);
}
}  // namespace animengine
