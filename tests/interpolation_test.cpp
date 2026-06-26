#include "animengine/interpolation.h"

#include "doctest.h"

TEST_CASE("linear interpolation") {
    CHECK(animengine::linear(0.0f, 10.0f, 0.0f) == doctest::Approx(0.0f));
    CHECK(animengine::linear(0.0f, 10.0f, 1.0f) == doctest::Approx(10.0f));
    CHECK(animengine::linear(0.0f, 10.0f, 0.5f) == doctest::Approx(5.0f));
    CHECK(animengine::linear(0.0f, 10.0f, 0.25f) == doctest::Approx(2.5f));
}
