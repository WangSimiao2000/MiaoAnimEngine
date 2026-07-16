#include "animengine/math.h"
#include "animengine/spring.h"
#include "doctest.h"

using animengine::Color;
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

TEST_CASE("Spring<Vec3> large update matches small updates") {
    constexpr float totalTime = 0.5f;
    constexpr int stepCount = 120;
    const Vec3 initial{1.0f, -2.0f, 0.5f};
    const Vec3 initialVelocity{0.5f, -1.0f, 2.0f};
    const Vec3 target{10.0f, -4.0f, 2.0f};
    Spring<Vec3> largeStep(0.5f, 0.6f, initial, initialVelocity);
    Spring<Vec3> smallSteps(0.5f, 0.6f, initial, initialVelocity);
    largeStep.setTarget(target);
    smallSteps.setTarget(target);

    largeStep.update(totalTime);
    for (int i = 0; i < stepCount; ++i) {
        smallSteps.update(totalTime / static_cast<float>(stepCount));
    }

    const Vec3 largeValue = largeStep.value();
    const Vec3 smallValue = smallSteps.value();
    const Vec3 largeVelocity = largeStep.velocity();
    const Vec3 smallVelocity = smallSteps.velocity();
    CHECK(largeValue.x == doctest::Approx(smallValue.x).epsilon(0.001));
    CHECK(largeValue.y == doctest::Approx(smallValue.y).epsilon(0.001));
    CHECK(largeValue.z == doctest::Approx(smallValue.z).epsilon(0.001));
    CHECK(largeVelocity.x == doctest::Approx(smallVelocity.x).epsilon(0.001));
    CHECK(largeVelocity.y == doctest::Approx(smallVelocity.y).epsilon(0.001));
    CHECK(largeVelocity.z == doctest::Approx(smallVelocity.z).epsilon(0.001));
}

TEST_CASE("Spring<Color> starts with an additive-zero velocity") {
    const Color initial{0.2f, 0.4f, 0.6f, 0.8f};
    Spring<Color> spring(0.5f, 0.0f, initial);

    const Color velocity = spring.velocity();
    CHECK(velocity.r == doctest::Approx(0.0f));
    CHECK(velocity.g == doctest::Approx(0.0f));
    CHECK(velocity.b == doctest::Approx(0.0f));
    CHECK(velocity.a == doctest::Approx(0.0f));
    CHECK(spring.isSettled());
}

TEST_CASE("Spring<Color> snapTo clears every velocity channel") {
    Spring<Color> spring(0.5f, 0.0f, Color{0.0f, 0.0f, 0.0f, 1.0f},
                         Color{1.0f, -1.0f, 0.5f, -0.5f});
    const Color snapped{0.2f, 0.4f, 0.6f, 0.8f};

    spring.snapTo(snapped);

    const Color velocity = spring.velocity();
    CHECK(velocity.r == doctest::Approx(0.0f));
    CHECK(velocity.g == doctest::Approx(0.0f));
    CHECK(velocity.b == doctest::Approx(0.0f));
    CHECK(velocity.a == doctest::Approx(0.0f));
    CHECK(spring.isSettled());
}
