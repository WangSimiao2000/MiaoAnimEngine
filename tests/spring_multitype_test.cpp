#include "animengine/math.h"
#include "animengine/spring.h"
#include "doctest.h"

using animengine::Spring;
using animengine::Vec3;

TEST_CASE("Spring<Vec3> converges to its target") {
    Spring<Vec3> s(0.4f, 0.0f, Vec3{0.0f, 0.0f, 0.0f});
    s.setTarget(Vec3{10.0f, -4.0f, 2.0f});
    for (int i = 0; i < 720; ++i) s.update(1.0f / 240.0f);  // 跑 3 秒
    const Vec3 v = s.value();
    CHECK(v.x == doctest::Approx(10.0f).epsilon(0.02));
    CHECK(v.y == doctest::Approx(-4.0f).epsilon(0.02));
    CHECK(v.z == doctest::Approx(2.0f).epsilon(0.02));
    CHECK(s.isSettled());
}
