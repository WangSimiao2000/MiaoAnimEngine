#include "animengine/spring.h"

#include <cmath>

#include "doctest.h"

using Spring = animengine::Spring<float>;

namespace {

// Advance a spring by `seconds` of simulated time using small fixed steps.
// A small step keeps the numerical integration accurate and stable.
void advance(Spring& spring, float seconds, float dt = 1.0f / 240.0f) {
    for (float elapsed = 0.0f; elapsed < seconds; elapsed += dt) {
        spring.update(dt);
    }
}

}  // namespace

TEST_CASE("spring starts at rest at its initial value") {
    Spring spring(0.5f, 0.0f, 3.0f);

    CHECK(spring.value() == doctest::Approx(3.0f));
    CHECK(spring.velocity() == doctest::Approx(0.0f));
    CHECK(spring.isSettled());
}

TEST_CASE("critically damped spring converges to the target without overshoot") {
    // bounce == 0  ->  damping ratio == 1  ->  no oscillation.
    Spring spring(0.5f, 0.0f, 0.0f);
    spring.setTarget(10.0f);

    float maxValue = 0.0f;
    for (float elapsed = 0.0f; elapsed < 2.0f; elapsed += 1.0f / 240.0f) {
        spring.update(1.0f / 240.0f);
        maxValue = std::fmax(maxValue, spring.value());
    }

    CHECK(spring.value() == doctest::Approx(10.0f).epsilon(0.01));
    CHECK(spring.isSettled());
    // A critically damped spring must not meaningfully overshoot the target.
    CHECK(maxValue <= doctest::Approx(10.0f).epsilon(0.01));
}

TEST_CASE("under-damped spring overshoots the target at least once") {
    // bounce > 0  ->  damping ratio < 1  ->  it should bounce past the target.
    Spring spring(0.5f, 0.6f, 0.0f);
    spring.setTarget(10.0f);

    float maxValue = 0.0f;
    for (float elapsed = 0.0f; elapsed < 3.0f; elapsed += 1.0f / 240.0f) {
        spring.update(1.0f / 240.0f);
        maxValue = std::fmax(maxValue, spring.value());
    }

    // The under-damped spring should overshoot the target before settling.
    CHECK(maxValue > 10.0f);
    // ...and still come to rest exactly on it.
    CHECK(spring.value() == doctest::Approx(10.0f).epsilon(0.01));
    CHECK(spring.isSettled());
}

TEST_CASE("setTarget keeps the current velocity (interruptible re-targeting)") {
    Spring spring(0.5f, 0.0f, 0.0f);
    spring.setTarget(10.0f);

    // Let it build up some momentum toward 10.
    advance(spring, 0.05f);
    const float valueBefore = spring.value();
    const float velocityBefore = spring.velocity();
    CHECK(velocityBefore > 0.0f);  // it is moving

    // Re-target mid-flight: value and velocity must be untouched by setTarget.
    spring.setTarget(-10.0f);

    CHECK(spring.value() == doctest::Approx(valueBefore));
    CHECK(spring.velocity() == doctest::Approx(velocityBefore));
}

TEST_CASE("isSettled is false while moving and true after it comes to rest") {
    Spring spring(0.5f, 0.0f, 0.0f);
    spring.setTarget(10.0f);

    spring.update(1.0f / 240.0f);
    CHECK_FALSE(spring.isSettled());  // just started, far from target

    advance(spring, 2.0f);
    CHECK(spring.isSettled());  // has come to rest at the target
}

TEST_CASE("snapTo settles at a value and remains there") {
    Spring spring(0.5f, 0.0f, 0.0f);
    spring.setTarget(10.0f);
    advance(spring, 0.05f);
    CHECK(spring.velocity() != doctest::Approx(0.0f));  // moving before snap

    spring.snapTo(5.0f);

    CHECK(spring.value() == doctest::Approx(5.0f));
    CHECK(spring.velocity() == doctest::Approx(0.0f));
    CHECK(spring.isSettled());

    // Advancing another frame must not resume motion toward the old target.
    spring.update(1.0f / 60.0f);

    CHECK(spring.value() == doctest::Approx(5.0f));
    CHECK(spring.velocity() == doctest::Approx(0.0f));
    CHECK(spring.isSettled());
}

TEST_CASE("initial velocity is respected") {
    // Target equals the initial value, but a positive initial velocity should
    // still push the value outward on the first update.
    Spring spring(0.5f, 0.0f, 0.0f, 2.0f);

    CHECK(spring.velocity() == doctest::Approx(2.0f));

    spring.update(0.01f);
    CHECK(spring.value() > 0.0f);
}
