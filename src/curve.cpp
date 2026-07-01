#include "animengine/curve.h"

#include <algorithm>

#include "animengine/easing.h"

namespace animengine {

void Curve::addKeyframe(const float time, const float value, const Easing easing) {
    const auto it = std::lower_bound(_keyframes.begin(), _keyframes.end(), time,
                                     [](const Keyframe& kf, const float t) { return kf.time < t; });
    if (it != _keyframes.end() && it->time == time) {
        it->value = value;
        it->easing = easing;
        return;
    }
    _keyframes.insert(it, Keyframe{time, value, easing});
}

float Curve::evaluate(const float time) const {
    if (_keyframes.empty()) {
        return 0.0f;
    }
    if (time <= _keyframes.front().time) {
        return _keyframes.front().value;
    }
    if (time >= _keyframes.back().time) {
        return _keyframes.back().value;
    }

    const auto next =
        std::upper_bound(_keyframes.begin(), _keyframes.end(), time,
                         [](const float t, const Keyframe& kf) { return t < kf.time; });
    const auto prev = next - 1;

    const float span = next->time - prev->time;
    const float t = (time - prev->time) / span;

    switch (prev->easing) {
        case Easing::StepStart:
            return stepStart(prev->value, next->value, t);
        case Easing::StepEnd:
            return stepEnd(prev->value, next->value, t);
        case Easing::Linear:
            return linear(prev->value, next->value, t);
        case Easing::EaseIn:
            return easeIn(prev->value, next->value, t);
        case Easing::EaseOut:
            return easeOut(prev->value, next->value, t);
        case Easing::EaseInOut:
            return easeInOut(prev->value, next->value, t);
    }

    return linear(prev->value, next->value, t);
}

}  // namespace animengine