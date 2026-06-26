#include "animengine/curve.h"

#include <algorithm>

#include "animengine/interpolation.h"

namespace animengine {

void Curve::addKeyframe(float time, float value, Interpolation interp) {
    const auto it = std::lower_bound(_keyframes.begin(), _keyframes.end(), time,
                                     [](const Keyframe& kf, float t) { return kf.time < t; });
    if (it != _keyframes.end() && it->time == time) {
        it->value = value;
        it->interp = interp;
        return;
    }
    _keyframes.insert(it, Keyframe{time, value, interp});
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

    // The segment [prev, next) is interpolated according to the left
    // keyframe's mode.
    // TODO(curve): when upgrading to the advanced (tangent-based) model, add a
    // Cubic case here that calls hermite(prev->value, prev->outTangent,
    // next->value, next->inTangent, t) -- see the note in curve.h.
    switch (prev->interp) {
        case Interpolation::Step:
            return prev->value;
        case Interpolation::Linear:
            break;
    }

    const float span = next->time - prev->time;
    const float t = (time - prev->time) / span;
    return linear(prev->value, next->value, t);
}

}  // namespace animengine