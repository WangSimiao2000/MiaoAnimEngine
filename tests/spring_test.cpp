#include "animengine/spring.h"

#include <cmath>
#include <limits>
#include <stdexcept>

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

TEST_CASE("spring rejects non-positive duration") {
    CHECK_THROWS_AS(Spring(0.0f, 0.0f), std::invalid_argument);
    CHECK_THROWS_AS(Spring(-0.5f, 0.0f), std::invalid_argument);
}

TEST_CASE("spring rejects non-finite duration") {
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float infinity = std::numeric_limits<float>::infinity();

    CHECK_THROWS_AS(Spring(nan, 0.0f), std::invalid_argument);
    CHECK_THROWS_AS(Spring(infinity, 0.0f), std::invalid_argument);
    CHECK_THROWS_AS(Spring(-infinity, 0.0f), std::invalid_argument);
}

TEST_CASE("spring accepts bounce inside the settling range") {
    CHECK_NOTHROW(Spring(0.5f, -0.5f));
    CHECK_NOTHROW(Spring(0.5f, 0.0f));
    CHECK_NOTHROW(Spring(0.5f, 0.6f));
}

TEST_CASE("spring rejects bounce outside the settling range") {
    CHECK_THROWS_AS(Spring(0.5f, -1.0f), std::invalid_argument);
    CHECK_THROWS_AS(Spring(0.5f, -1.1f), std::invalid_argument);
    CHECK_THROWS_AS(Spring(0.5f, 1.0f), std::invalid_argument);
    CHECK_THROWS_AS(Spring(0.5f, 1.1f), std::invalid_argument);
}

TEST_CASE("spring rejects non-finite bounce") {
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float infinity = std::numeric_limits<float>::infinity();

    CHECK_THROWS_AS(Spring(0.5f, nan), std::invalid_argument);
    CHECK_THROWS_AS(Spring(0.5f, infinity), std::invalid_argument);
    CHECK_THROWS_AS(Spring(0.5f, -infinity), std::invalid_argument);
}

TEST_CASE("update rejects invalid delta time") {
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float infinity = std::numeric_limits<float>::infinity();

    CHECK_THROWS_AS(Spring(0.5f, 0.0f).update(-0.01f), std::invalid_argument);
    CHECK_THROWS_AS(Spring(0.5f, 0.0f).update(nan), std::invalid_argument);
    CHECK_THROWS_AS(Spring(0.5f, 0.0f).update(infinity), std::invalid_argument);
    CHECK_THROWS_AS(Spring(0.5f, 0.0f).update(-infinity), std::invalid_argument);
}

TEST_CASE("update with zero delta time leaves state unchanged") {
    Spring spring(0.5f, 0.6f, 2.0f, 3.0f);
    spring.setTarget(10.0f);
    const float valueBefore = spring.value();
    const float velocityBefore = spring.velocity();

    CHECK_NOTHROW(spring.update(0.0f));
    CHECK(spring.value() == doctest::Approx(valueBefore));
    CHECK(spring.velocity() == doctest::Approx(velocityBefore));
}

TEST_CASE("critically damped spring matches the closed-form solution") {
    constexpr double pi = 3.14159265358979323846;
    constexpr double time = 0.25;
    constexpr double omega = 2.0 * pi;
    const double decay = std::exp(-omega * time);
    const double expectedValue = 1.0 - (1.0 + omega * time) * decay;
    const double expectedVelocity = omega * omega * time * decay;

    Spring spring(1.0f, 0.0f, 0.0f);
    spring.setTarget(1.0f);
    spring.update(static_cast<float>(time));

    CHECK(spring.value() == doctest::Approx(expectedValue).epsilon(0.00001));
    CHECK(spring.velocity() == doctest::Approx(expectedVelocity).epsilon(0.00001));
}

TEST_CASE("underdamped spring matches an independent closed-form result") {
    Spring spring(1.0f, 0.5f, 2.0f, 3.0f);
    spring.setTarget(10.0f);
    spring.update(0.25f);

    CHECK(spring.value() == doctest::Approx(7.4244464224).epsilon(0.00001));
    CHECK(spring.velocity() == doctest::Approx(25.3929726785).epsilon(0.00001));
}

TEST_CASE("overdamped spring matches an independent closed-form result") {
    Spring spring(1.0f, -0.5f, 2.0f, 3.0f);
    spring.setTarget(10.0f);
    spring.update(0.25f);

    CHECK(spring.value() == doctest::Approx(4.4339512098).epsilon(0.00001));
    CHECK(spring.velocity() == doctest::Approx(9.3410772225).epsilon(0.00001));
}

TEST_CASE("near-critical springs preserve their actual damping ratio") {
    struct ExpectedState {
        float bounce;
        double value;
        double velocity;
    };
    const ExpectedState states[] = {
        {-0.00005f, 5.8804680095, 16.0569442454},
        {0.00005f, 5.8806945348, 16.0581412848},
    };

    for (const ExpectedState& state : states) {
        CAPTURE(state.bounce);
        Spring spring(1.0f, state.bounce, 2.0f, 3.0f);
        spring.setTarget(10.0f);
        spring.update(0.25f);

        CHECK(spring.value() == doctest::Approx(state.value).epsilon(0.000005));
        CHECK(spring.velocity() == doctest::Approx(state.velocity).epsilon(0.000005));
    }
}

TEST_CASE("analytic update remains finite for an extreme finite time scale") {
    constexpr double pi = 3.14159265358979323846;
    const float tinyTime = std::numeric_limits<float>::denorm_min();
    const double expectedVelocity = 4.0 * pi * pi * std::exp(-2.0 * pi);
    Spring spring(tinyTime, 0.0f, 0.0f);
    spring.setTarget(tinyTime);

    spring.update(tinyTime);

    CHECK(std::isfinite(spring.value()));
    CHECK(std::isfinite(spring.velocity()));
    CHECK(spring.velocity() == doctest::Approx(expectedVelocity).epsilon(0.001));
}

TEST_CASE("large update matches small updates in every damping regime") {
    constexpr float totalTime = 0.5f;
    constexpr int stepCount = 120;
    const float bounces[] = {-0.5f, 0.0f, 0.6f};

    for (const float bounce : bounces) {
        CAPTURE(bounce);
        Spring largeStep(0.5f, bounce, 2.0f, 3.0f);
        Spring smallSteps(0.5f, bounce, 2.0f, 3.0f);
        largeStep.setTarget(10.0f);
        smallSteps.setTarget(10.0f);

        largeStep.update(totalTime);
        for (int i = 0; i < stepCount; ++i) {
            smallSteps.update(totalTime / static_cast<float>(stepCount));
        }

        CHECK(std::isfinite(largeStep.value()));
        CHECK(std::isfinite(largeStep.velocity()));
        CHECK(largeStep.value() == doctest::Approx(smallSteps.value()).epsilon(0.001));
        CHECK(largeStep.velocity() == doctest::Approx(smallSteps.velocity()).epsilon(0.001));
    }
}

TEST_CASE("overdamped spring remains monotonic with large updates") {
    Spring spring(0.5f, -0.5f, 0.0f);
    spring.setTarget(10.0f);

    float previousValue = spring.value();
    for (int i = 0; i < 30; ++i) {
        spring.update(0.1f);
        CHECK(spring.value() + 0.00001f >= previousValue);
        CHECK(spring.value() <= 10.00001f);
        previousValue = spring.value();
    }

    CHECK(spring.value() == doctest::Approx(10.0f).epsilon(0.01));
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
