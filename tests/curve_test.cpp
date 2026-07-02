#include "animengine/curve.h"

#include "doctest.h"

using Curve = animengine::Curve<float>;
using animengine::Easing;

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
    CHECK(curve.evaluate(-1.0f) == doctest::Approx(0.0f));  // before first
    CHECK(curve.evaluate(5.0f) == doctest::Approx(20.0f));  // after last
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
    CHECK(curve.evaluate(1.0f) == doctest::Approx(10.0f));  // midpoint
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
    CHECK(curve.evaluate(1.0f) == doctest::Approx(5.0f));   // segment [0,2]
    CHECK(curve.evaluate(2.0f) == doctest::Approx(10.0f));  // shared keyframe
    CHECK(curve.evaluate(3.0f) == doctest::Approx(5.0f));   // segment [2,4]
}

TEST_CASE("step interpolation holds the left keyframe's value across the segment") {
    Curve curve;
    curve.addKeyframe(0.0f, 0.0f, Easing::StepEnd);
    curve.addKeyframe(2.0f, 20.0f);  // last keyframe: interp ignored

    CHECK(curve.evaluate(0.0f) == doctest::Approx(0.0f));   // on the left keyframe
    CHECK(curve.evaluate(0.5f) == doctest::Approx(0.0f));   // holds prev value
    CHECK(curve.evaluate(1.9f) == doctest::Approx(0.0f));   // still holds, just before next
    CHECK(curve.evaluate(2.0f) == doctest::Approx(20.0f));  // jumps on reaching next
    CHECK(curve.evaluate(5.0f) == doctest::Approx(20.0f));  // clamps to last
}

TEST_CASE("step mode is per-segment, decided by the left keyframe") {
    Curve curve;
    curve.addKeyframe(0.0f, 0.0f, Easing::Linear);    // segment [0,2) interpolates
    curve.addKeyframe(2.0f, 10.0f, Easing::StepEnd);  // segment [2,4) holds
    curve.addKeyframe(4.0f, 20.0f);

    CHECK(curve.evaluate(1.0f) == doctest::Approx(5.0f));   // linear segment, midpoint
    CHECK(curve.evaluate(3.0f) == doctest::Approx(10.0f));  // step segment, holds prev
    CHECK(curve.evaluate(4.0f) == doctest::Approx(20.0f));  // reaches next keyframe
}

TEST_CASE("interp defaults to linear, preserving the original behaviour") {
    Curve curve;
    curve.addKeyframe(0.0f, 0.0f);  // no interp argument
    curve.addKeyframe(2.0f, 20.0f);
    CHECK(curve.evaluate(1.0f) == doctest::Approx(10.0f));
}

TEST_CASE("overwriting a keyframe also updates its interpolation mode") {
    Curve curve;
    curve.addKeyframe(0.0f, 0.0f, Easing::Linear);
    curve.addKeyframe(2.0f, 20.0f);
    CHECK(curve.evaluate(1.0f) == doctest::Approx(10.0f));  // linear before overwrite

    curve.addKeyframe(0.0f, 0.0f, Easing::StepEnd);        // switch segment to step
    CHECK(curve.evaluate(1.0f) == doctest::Approx(0.0f));  // now holds prev value
}

TEST_CASE("easeIn interpolation accelerates from start") {
    Curve curve;
    curve.addKeyframe(0.0f, 0.0f, Easing::EaseIn);
    curve.addKeyframe(1.0f, 1.0f);

    CHECK(curve.evaluate(0.0f) == doctest::Approx(0.0f));
    CHECK(curve.evaluate(0.5f) == doctest::Approx(0.25f));  // t² at midpoint
    CHECK(curve.evaluate(1.0f) == doctest::Approx(1.0f));
}

TEST_CASE("easeOut interpolation decelerates to end") {
    Curve curve;
    curve.addKeyframe(0.0f, 0.0f, Easing::EaseOut);
    curve.addKeyframe(1.0f, 1.0f);

    CHECK(curve.evaluate(0.0f) == doctest::Approx(0.0f));
    CHECK(curve.evaluate(0.5f) == doctest::Approx(0.75f));  // 1-(1-t)²
    CHECK(curve.evaluate(1.0f) == doctest::Approx(1.0f));
}

TEST_CASE("easeInOut interpolation is symmetric") {
    Curve curve;
    curve.addKeyframe(0.0f, 0.0f, Easing::EaseInOut);
    curve.addKeyframe(1.0f, 1.0f);

    CHECK(curve.evaluate(0.0f) == doctest::Approx(0.0f));
    CHECK(curve.evaluate(0.25f) == doctest::Approx(0.125f));  // first half: easeIn
    CHECK(curve.evaluate(0.5f) == doctest::Approx(0.5f));
    CHECK(curve.evaluate(0.75f) == doctest::Approx(0.875f));  // second half: easeOut
    CHECK(curve.evaluate(1.0f) == doctest::Approx(1.0f));
}

TEST_CASE("different easing modes on different segments") {
    Curve curve;
    curve.addKeyframe(0.0f, 0.0f, Easing::EaseIn);
    curve.addKeyframe(1.0f, 10.0f, Easing::EaseOut);
    curve.addKeyframe(2.0f, 20.0f);

    // First segment [0,1]: EaseIn
    CHECK(curve.evaluate(0.5f) == doctest::Approx(2.5f));  // 0.5² * 10 = 2.5

    // Second segment [1,2]: EaseOut
    CHECK(curve.evaluate(1.5f) == doctest::Approx(17.5f));  // 10 + 0.75*10 = 17.5
}
