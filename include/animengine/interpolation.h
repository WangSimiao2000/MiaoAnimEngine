#pragma once
#include "easing.h"

namespace animengine {
template <class T>
[[nodiscard]] T lerp(const T& a, const T& b, float t) {
    return a + (b - a) * t;
}

template <class T>
[[nodiscard]] T ease(const T& from, const T& to, float t, Easing easing) {
    return lerp(from, to, easeShape(easing, t));
}
}  // namespace animengine
