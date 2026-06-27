#pragma once

namespace animengine {

enum class Easing {
    StepStart,
    StepEnd,
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
};

float stepStart(float start, float end, float t);
float stepEnd(float start, float end, float t);
float linear(float start, float end, float t);
float easeIn(float start, float end, float t);
float easeOut(float start, float end, float t);
float easeInOut(float start, float end, float t);

}  // namespace animengine
