#include "animengine/interpolation.h"

#include "doctest.h"

using animengine::ease;
using animengine::lerp;

TEST_CASE("lerp<float> endpoints and midpoint") {
    CHECK(lerp(0.0f, 10.0f, 0.0f) == doctest::Approx(0.0f));
    CHECK(lerp(0.0f, 10.0f, 1.0f) == doctest::Approx(10.0f));
    CHECK(lerp(0.0f, 10.0f, 0.5f) == doctest::Approx(5.0f));
}

TEST_CASE("ease<float> matches easeShape composition") {
    // EaseIn at t=0.5 -> shape 0.25 -> value 2.5 on [0,10]
    CHECK(ease(0.0f, 10.0f, 0.5f, animengine::Easing::EaseIn) == doctest::Approx(2.5f));
}