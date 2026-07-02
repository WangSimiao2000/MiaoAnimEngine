#include <cmath>

#include "animengine/math.h"
#include "animengine/tween.h"
#include "doctest.h"

using animengine::Quat;
using animengine::Tween;
using animengine::Vec3;

TEST_CASE("Tween<Vec3> interpolates each component over time") {
    Tween<Vec3> t(Vec3{0.0f, 0.0f, 0.0f}, Vec3{10.0f, -20.0f, 4.0f}, 1.0f);
    t.update(0.5f);
    const Vec3 v = t.value();
    CHECK(v.x == doctest::Approx(5.0f));
    CHECK(v.y == doctest::Approx(-10.0f));
    CHECK(v.z == doctest::Approx(2.0f));
}

TEST_CASE("Tween<Quat> drives a slerp and stays unit-length") {
    const Quat from{0.0f, 0.0f, 0.0f, 1.0f};                    // identity
    const Quat to{0.0f, 0.0f, std::sin(0.5f), std::cos(0.5f)};  // ~57deg about Z
    Tween<Quat> t(from, to, 1.0f);
    t.update(0.5f);
    const Quat q = t.value();
    CHECK(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w == doctest::Approx(1.0f).epsilon(0.001));
}
