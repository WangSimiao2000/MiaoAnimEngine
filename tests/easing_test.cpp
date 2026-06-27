#include "animengine/easing.h"

#include "doctest.h"

TEST_CASE("stepStart") {
    CHECK(animengine::stepStart(0.0f, 10.0f, 0.0f) == doctest::Approx(0.0f));
    CHECK(animengine::stepStart(0.0f, 10.0f, 0.001f) == doctest::Approx(10.0f));
    CHECK(animengine::stepStart(0.0f, 10.0f, 0.5f) == doctest::Approx(10.0f));
}

TEST_CASE("stepEnd") {
    CHECK(animengine::stepEnd(0.0f, 10.0f, 0.0f) == doctest::Approx(0.0f));
    CHECK(animengine::stepEnd(0.0f, 10.0f, 0.5f) == doctest::Approx(0.0f));
    CHECK(animengine::stepEnd(0.0f, 10.0f, 1.0f) == doctest::Approx(10.0f));
}

TEST_CASE("linear interpolation") {
    CHECK(animengine::linear(0.0f, 10.0f, 0.0f) == doctest::Approx(0.0f));
    CHECK(animengine::linear(0.0f, 10.0f, 1.0f) == doctest::Approx(10.0f));
    CHECK(animengine::linear(0.0f, 10.0f, 0.5f) == doctest::Approx(5.0f));
}

TEST_CASE("easeIn quadratic") {
    CHECK(animengine::easeIn(0.0f, 1.0f, 0.0f) == doctest::Approx(0.0f));
    CHECK(animengine::easeIn(0.0f, 1.0f, 1.0f) == doctest::Approx(1.0f));
    CHECK(animengine::easeIn(0.0f, 1.0f, 0.5f) == doctest::Approx(0.25f));  // 0.5² = 0.25
}

TEST_CASE("easeOut quadratic") {
    CHECK(animengine::easeOut(0.0f, 1.0f, 0.0f) == doctest::Approx(0.0f));
    CHECK(animengine::easeOut(0.0f, 1.0f, 1.0f) == doctest::Approx(1.0f));
    CHECK(animengine::easeOut(0.0f, 1.0f, 0.5f) == doctest::Approx(0.75f));  // 1-(1-0.5)² = 0.75
}

TEST_CASE("easeInOut quadratic") {
    CHECK(animengine::easeInOut(0.0f, 1.0f, 0.0f) == doctest::Approx(0.0f));
    CHECK(animengine::easeInOut(0.0f, 1.0f, 1.0f) == doctest::Approx(1.0f));
    CHECK(animengine::easeInOut(0.0f, 1.0f, 0.25f) == doctest::Approx(0.125f));  // 2*(0.25) ²
    CHECK(animengine::easeInOut(0.0f, 1.0f, 0.5f) == doctest::Approx(0.5f));
    CHECK(animengine::easeInOut(0.0f, 1.0f, 0.75f) == doctest::Approx(0.875f));
}
