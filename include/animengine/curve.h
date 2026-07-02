#pragma once

#include <algorithm>
#include <vector>

#include "easing.h"
#include "interpolation.h"
#include "math.h"

namespace animengine {

template <class T = float>
struct Keyframe {
    float time{};
    T value{};
    Easing easing = Easing::Linear;
};

template <class T = float>
class Curve {
public:
    void addKeyframe(float time, T value, Easing easing = Easing::Linear) {
        const auto it =
            std::lower_bound(_keyframes.begin(), _keyframes.end(), time,
                             [](const Keyframe<T>& kf, float t) { return kf.time < t; });
        if (it != _keyframes.end() && it->time == time) {
            it->value = value;
            it->easing = easing;
            return;
        }
        _keyframes.insert(it, Keyframe<T>{time, value, easing});
    }

    [[nodiscard]] T evaluate(float time) const {
        if (_keyframes.empty()) {
            return T{};
        }
        if (time <= _keyframes.front().time) {
            return _keyframes.front().value;
        }
        if (time >= _keyframes.back().time) {
            return _keyframes.back().value;
        }

        const auto next =
            std::upper_bound(_keyframes.begin(), _keyframes.end(), time,
                             [](float t, const Keyframe<T>& kf) { return t < kf.time; });
        const auto prev = next - 1;

        const float span = next->time - prev->time;
        const float t = (time - prev->time) / span;

        return ease(prev->value, next->value, t, prev->easing);
    }

private:
    std::vector<Keyframe<T>> _keyframes;
};

using CurveFloat = Curve<float>;
using CurveVec3 = Curve<Vec3>;
using CurveColor = Curve<Color>;
using CurveQuat = Curve<Quat>;

}  // namespace animengine