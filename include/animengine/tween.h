#pragma once

#include "easing.h"

namespace animengine {

class Tween {
public:
    Tween(float from, float to, float duration, Easing easing = Easing::Linear);

    void update(float deltaTime);
    void reset();

    float value() const;
    float elapsed() const;
    float duration() const;

    bool isFinished() const;

private:
    float _from, _to, _duration;
    float _elapsed = 0.0f;
    Easing _easing = Easing::Linear;
};

}  // namespace animengine
