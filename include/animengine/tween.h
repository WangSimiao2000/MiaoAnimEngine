#pragma once

#include "easing.h"
#include "interpolation.h"
#include "math.h"

namespace animengine {
template <class T = float>
class Tween {
public:
    Tween(T from, T to, float duration, Easing easing = Easing::Linear)
        : _from(from), _to(to), _duration(duration), _easing(easing) {}

    void update(float deltaTime) {
        if (deltaTime <= 0.0f || isFinished()) {
            return;
        }
        _elapsed += deltaTime;
        if (_elapsed > _duration) {
            _elapsed = _duration;
        }
    }
    void reset() {
        _elapsed = 0.0f;
    }

    [[nodiscard]] T value() const {
        if (_duration <= 0.0f) {
            return _to;
        }
        return ease(_from, _to, elapsed() / _duration, _easing);
    }

    [[nodiscard]] float elapsed() const {
        if (_duration <= 0.0f || _elapsed <= 0.0f) {
            return 0.0f;
        } else if (_elapsed >= _duration) {
            return _duration;
        } else {
            return _elapsed;
        }
    }
    [[nodiscard]] float duration() const {
        return _duration;
    }

    [[nodiscard]] bool isFinished() const {
        return _duration <= 0.0f || _elapsed >= _duration;
    }

private:
    T _from, _to;
    float _duration;
    float _elapsed = 0.0f;
    Easing _easing = Easing::Linear;
};

using TweenFloat = Tween<float>;
using TweenVec3 = Tween<Vec3>;
using TweenColor = Tween<Color>;
using TweenQuat = Tween<Quat>;

}  // namespace animengine
