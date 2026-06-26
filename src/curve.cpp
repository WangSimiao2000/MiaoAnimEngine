#include "animengine/curve.h"

#include <algorithm>

#include "animengine/interpolation.h"

namespace animengine {

void Curve::addKeyframe(float time, float value) {
    const auto it = std::lower_bound(_keyframes.begin(), _keyframes.end(), time,
                                     [](const Keyframe& kf, float t) { return kf.time < t; });
    if (it != _keyframes.end() && it->time == time) {
        it->value = value;
        return;
    }
    _keyframes.insert(it, Keyframe{time, value});
}

float Curve::evaluate(float time) const {
    if (_keyframes.empty()) {
        return 0.0f;
    }
    if (time <= _keyframes.front().time) {
        return _keyframes.front().value;
    }
    if (time >= _keyframes.back().time) {
        return _keyframes.back().value;
    }

    const auto next = std::upper_bound(_keyframes.begin(), _keyframes.end(), time,
                                       [](float t, const Keyframe& kf) { return t < kf.time; });
    const auto prev = next - 1;

    const float span = next->time - prev->time;
    const float t = (time - prev->time) / span;
    return linear(prev->value, next->value, t);
}

}  // namespace animengine