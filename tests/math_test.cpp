#include "animengine/math.h"

#include "animengine/interpolation.h"
#include "doctest.h"

using animengine::Color;
using animengine::lerp;
using animengine::Vec3;

TEST_CASE("lerp<Vec3> interpolates each component") {
    const Vec3 a{0.0f, 0.0f, 0.0f};
    const Vec3 b{10.0f, -20.0f, 4.0f};

    SUBCASE("endpoints are returned exactly") {
        const Vec3 start = lerp(a, b, 0.0f);
        CHECK(start.x == doctest::Approx(0.0f));
        CHECK(start.y == doctest::Approx(0.0f));
        CHECK(start.z == doctest::Approx(0.0f));

        const Vec3 end = lerp(a, b, 1.0f);
        CHECK(end.x == doctest::Approx(10.0f));
        CHECK(end.y == doctest::Approx(-20.0f));
        CHECK(end.z == doctest::Approx(4.0f));
    }

    SUBCASE("midpoint is the component-wise average") {
        const Vec3 mid = lerp(a, b, 0.5f);
        CHECK(mid.x == doctest::Approx(5.0f));
        CHECK(mid.y == doctest::Approx(-10.0f));
        CHECK(mid.z == doctest::Approx(2.0f));
    }
}

TEST_CASE("lerp<Color> interpolates each channel") {
    const Color from{0.0f, 0.0f, 0.0f, 0.0f};
    const Color to{1.0f, 0.5f, 0.25f, 1.0f};

    const Color mid = lerp(from, to, 0.5f);
    CHECK(mid.r == doctest::Approx(0.5f));
    CHECK(mid.g == doctest::Approx(0.25f));
    CHECK(mid.b == doctest::Approx(0.125f));
    CHECK(mid.a == doctest::Approx(0.5f));

    const Color end = lerp(from, to, 1.0f);
    CHECK(end.r == doctest::Approx(1.0f));
    CHECK(end.g == doctest::Approx(0.5f));
    CHECK(end.b == doctest::Approx(0.25f));
    CHECK(end.a == doctest::Approx(1.0f));
}
