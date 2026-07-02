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

[[nodiscard]] float easeShape(Easing easing, float t);

[[nodiscard]] float stepStart(float start, float end, float t);
[[nodiscard]] float stepEnd(float start, float end, float t);
[[nodiscard]] float linear(float start, float end, float t);
[[nodiscard]] float easeIn(float start, float end, float t);
[[nodiscard]] float easeOut(float start, float end, float t);
[[nodiscard]] float easeInOut(float start, float end, float t);

}  // namespace animengine
