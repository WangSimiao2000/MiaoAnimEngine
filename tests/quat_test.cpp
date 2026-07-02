#include <cmath>

#include "animengine/interpolation.h"
#include "animengine/math.h"
#include "doctest.h"

using animengine::lerp;
using animengine::Quat;

namespace {
constexpr float kPi = 3.14159265358979323846f;

// Unit quaternion for a rotation of `angleRad` about the Z axis.
Quat rotZ(float angleRad) {
    return Quat{0.0f, 0.0f, std::sin(angleRad * 0.5f), std::cos(angleRad * 0.5f)};
}
float len2(const Quat& q) {
    return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}
}  // namespace

TEST_CASE("slerp returns the endpoints") {
    const Quat a = rotZ(0.0f);        // identity
    const Quat b = rotZ(kPi * 0.5f);  // 90° about Z, same hemisphere (dot > 0)

    const Quat s0 = lerp(a, b, 0.0f);
    CHECK(s0.z == doctest::Approx(a.z));
    CHECK(s0.w == doctest::Approx(a.w));

    const Quat s1 = lerp(a, b, 1.0f);
    CHECK(s1.z == doctest::Approx(b.z));
    CHECK(s1.w == doctest::Approx(b.w));
}

TEST_CASE("slerp midpoint has constant angular velocity") {
    // Halfway between 0° and 90° must be exactly 45° about Z.
    const Quat mid = lerp(rotZ(0.0f), rotZ(kPi * 0.5f), 0.5f);
    const Quat expected = rotZ(kPi * 0.25f);  // 45°
    CHECK(mid.z == doctest::Approx(expected.z));
    CHECK(mid.w == doctest::Approx(expected.w));
}

TEST_CASE("slerp keeps the result unit-length") {
    const Quat a = rotZ(0.3f);
    const Quat b = rotZ(2.0f);
    for (float t = 0.0f; t <= 1.0f; t += 0.1f) {
        CHECK(len2(lerp(a, b, t)) == doctest::Approx(1.0f).epsilon(0.001));
    }
}

TEST_CASE("slerp takes the shortest arc") {
    // +170° and -170° are only 20° apart the short way (through 180°), but
    // 340° apart the long way. Shortest-path slerp must pass through ~180°
    // (z ~= +/-1, w ~= 0), NOT through identity (w ~= 1).
    const Quat a = rotZ(170.0f * kPi / 180.0f);
    const Quat b = rotZ(-170.0f * kPi / 180.0f);
    const Quat mid = lerp(a, b, 0.5f);
    CHECK(std::fabs(mid.z) == doctest::Approx(1.0f).epsilon(0.01));
    CHECK(mid.w == doctest::Approx(0.0f).epsilon(0.01));
}

TEST_CASE("slerp handles nearly identical quaternions (degenerate branch)") {
    const Quat mid = lerp(rotZ(0.5f), rotZ(0.5001f), 0.5f);
    CHECK(len2(mid) == doctest::Approx(1.0f).epsilon(0.001));  // must not blow up
}
