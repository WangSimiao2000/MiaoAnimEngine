#include "animengine/tween.h"

namespace animengine {

Tween::Tween(float from, float to, float duration, Easing easing)
    : _from(from), _to(to), _duration(duration), _easing(easing) {}

void Tween::update(float deltaTime) {
    if (deltaTime <= 0.0f || isFinished()) {
        return;
    }
    _elapsed += deltaTime;
    if (_elapsed > _duration) {
        _elapsed = _duration;
    }
}

void Tween::reset() {
    _elapsed = 0.0f;
}

float Tween::value() const {
    if (_duration <= 0.0f) {
        return _to;
    }

    const float t = elapsed() / _duration;

    switch (_easing) {
        case Easing::StepStart:
            return stepStart(_from, _to, t);
        case Easing::StepEnd:
            return stepEnd(_from, _to, t);
        case Easing::Linear:
            return linear(_from, _to, t);
        case Easing::EaseIn:
            return easeIn(_from, _to, t);
        case Easing::EaseOut:
            return easeOut(_from, _to, t);
        case Easing::EaseInOut:
            return easeInOut(_from, _to, t);
    }

    return linear(_from, _to, t);
}

float Tween::elapsed() const {
    if (_duration <= 0.0f) {
        return 0.0f;
    } else if (_elapsed <= 0.0f) {
        return 0.0f;
    } else if (_elapsed >= _duration) {
        return _duration;
    } else {
        return _elapsed;
    }
}

float Tween::duration() const {
    return _duration;
}

bool Tween::isFinished() const {
    return _duration <= 0.0f || _elapsed >= _duration;
}

}  // namespace animengine
