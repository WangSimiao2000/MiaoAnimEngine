#include "animengine/curve.h"

#include "doctest.h"

using animengine::Curve;

TEST_CASE("empty curve returns the default value") {
    Curve curve;
    CHECK(curve.evaluate(0.0f) == doctest::Approx(0.0f));
    CHECK(curve.evaluate(123.0f) == doctest::Approx(0.0f));
}

TEST_CASE("single keyframe returns its value everywhere") {
    Curve curve;
    curve.addKeyframe(1.0f, 5.0f);
    CHECK(curve.evaluate(-10.0f) == doctest::Approx(5.0f));
    CHECK(curve.evaluate(1.0f) == doctest::Approx(5.0f));
    CHECK(curve.evaluate(10.0f) == doctest::Approx(5.0f));
}

TEST_CASE("clamps outside the keyframe range") {
    Curve curve;
    curve.addKeyframe(0.0f, 0.0f);
    curve.addKeyframe(2.0f, 20.0f);
    CHECK(curve.evaluate(-1.0f) == doctest::Approx(0.0f));   // before first
    CHECK(curve.evaluate(5.0f) == doctest::Approx(20.0f));   // after last
}

TEST_CASE("hits exact keyframe values") {
    Curve curve;
    curve.addKeyframe(0.0f, 0.0f);
    curve.addKeyframe(2.0f, 20.0f);
    CHECK(curve.evaluate(0.0f) == doctest::Approx(0.0f));
    CHECK(curve.evaluate(2.0f) == doctest::Approx(20.0f));
}

TEST_CASE("interpolates linearly between two keyframes") {
    Curve curve;
    curve.addKeyframe(0.0f, 0.0f);
    curve.addKeyframe(2.0f, 20.0f);
    CHECK(curve.evaluate(1.0f) == doctest::Approx(10.0f));   // midpoint
    CHECK(curve.evaluate(0.5f) == doctest::Approx(5.0f));
    CHECK(curve.evaluate(1.5f) == doctest::Approx(15.0f));
}

TEST_CASE("adding at an existing time overwrites the value") {
    Curve curve;
    curve.addKeyframe(0.0f, 0.0f);
    curve.addKeyframe(2.0f, 20.0f);
    curve.addKeyframe(2.0f, 99.0f);  // overwrites the keyframe at t = 2

    CHECK(curve.evaluate(2.0f) == doctest::Approx(99.0f));
    CHECK(curve.evaluate(1.0f) == doctest::Approx(49.5f));  // midpoint of 0..99
    CHECK(curve.evaluate(5.0f) == doctest::Approx(99.0f));  // clamps to last
}

TEST_CASE("interpolates across multiple segments regardless of insert order") {
    Curve curve;
    // Inserted out of order; addKeyframe must keep them sorted by time.
    curve.addKeyframe(4.0f, 0.0f);
    curve.addKeyframe(0.0f, 0.0f);
    curve.addKeyframe(2.0f, 10.0f);
    CHECK(curve.evaluate(1.0f) == doctest::Approx(5.0f));    // segment [0,2]
    CHECK(curve.evaluate(2.0f) == doctest::Approx(10.0f));   // shared keyframe
    CHECK(curve.evaluate(3.0f) == doctest::Approx(5.0f));    // segment [2,4]
}
