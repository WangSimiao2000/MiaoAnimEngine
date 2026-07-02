#include "animengine/curve.h"
#include "animengine/math.h"
#include "doctest.h"

using animengine::Curve;
using animengine::Vec3;

TEST_CASE("Curve<Vec3> interpolates keyframes component-wise") {
    Curve<Vec3> c;
    c.addKeyframe(0.0f, Vec3{0.0f, 0.0f, 0.0f});
    c.addKeyframe(2.0f, Vec3{20.0f, -10.0f, 4.0f});

    const Vec3 v = c.evaluate(1.0f);  // midpoint
    CHECK(v.x == doctest::Approx(10.0f));
    CHECK(v.y == doctest::Approx(-5.0f));
    CHECK(v.z == doctest::Approx(2.0f));
}
