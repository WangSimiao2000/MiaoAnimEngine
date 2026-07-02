#include "animengine/tween.h"

#include <type_traits>

#include "doctest.h"

using animengine::Easing;
using animengine::Tween;

// Contract: a bare `Tween(float args)` must deduce to Tween<float> via CTAD.
// This guards the zero-friction API against future template changes that would
// break class template argument deduction.
static_assert(std::is_same_v<decltype(Tween(0.0f, 1.0f, 1.0f)), Tween<float>>,
              "CTAD for Tween should deduce Tween<float>");

TEST_CASE("tween starts at the from value") {
    Tween tween(0.0f, 10.0f, 1.0f);

    CHECK(tween.value() == doctest::Approx(0.0f));
    CHECK(tween.elapsed() == doctest::Approx(0.0f));
    CHECK(tween.duration() == doctest::Approx(1.0f));
    CHECK_FALSE(tween.isFinished());
}

TEST_CASE("linear tween reaches midpoint after half duration") {
    Tween tween(0.0f, 10.0f, 1.0f);

    tween.update(0.5f);

    CHECK(tween.elapsed() == doctest::Approx(0.5f));
    CHECK(tween.value() == doctest::Approx(5.0f));
    CHECK_FALSE(tween.isFinished());
}

TEST_CASE("update clamps elapsed time at duration") {
    Tween tween(0.0f, 10.0f, 1.0f);

    tween.update(2.0f);

    CHECK(tween.elapsed() == doctest::Approx(1.0f));
    CHECK(tween.value() == doctest::Approx(10.0f));
    CHECK(tween.isFinished());
}

TEST_CASE("finished tween ignores further updates") {
    Tween tween(0.0f, 10.0f, 1.0f);

    tween.update(1.0f);
    tween.update(0.5f);

    CHECK(tween.elapsed() == doctest::Approx(1.0f));
    CHECK(tween.value() == doctest::Approx(10.0f));
    CHECK(tween.isFinished());
}

TEST_CASE("negative delta time is ignored") {
    Tween tween(0.0f, 10.0f, 1.0f);

    tween.update(0.5f);
    tween.update(-0.25f);

    CHECK(tween.elapsed() == doctest::Approx(0.5f));
    CHECK(tween.value() == doctest::Approx(5.0f));
}

TEST_CASE("reset returns tween to the start") {
    Tween tween(0.0f, 10.0f, 1.0f);

    tween.update(1.0f);
    tween.reset();

    CHECK(tween.elapsed() == doctest::Approx(0.0f));
    CHECK(tween.value() == doctest::Approx(0.0f));
    CHECK_FALSE(tween.isFinished());
}

TEST_CASE("zero duration returns target value and is finished") {
    Tween tween(0.0f, 10.0f, 0.0f);

    CHECK(tween.elapsed() == doctest::Approx(0.0f));
    CHECK(tween.value() == doctest::Approx(10.0f));
    CHECK(tween.isFinished());

    tween.update(1.0f);

    CHECK(tween.elapsed() == doctest::Approx(0.0f));
    CHECK(tween.value() == doctest::Approx(10.0f));
}

TEST_CASE("negative duration behaves like an already finished tween") {
    Tween tween(0.0f, 10.0f, -1.0f);

    CHECK(tween.elapsed() == doctest::Approx(0.0f));
    CHECK(tween.value() == doctest::Approx(10.0f));
    CHECK(tween.isFinished());
}

TEST_CASE("easeOut tween uses easing function") {
    Tween tween(0.0f, 10.0f, 1.0f, Easing::EaseOut);

    tween.update(0.5f);

    CHECK(tween.value() == doctest::Approx(7.5f));
}

TEST_CASE("stepEnd tween holds start value until it finishes") {
    Tween tween(0.0f, 10.0f, 1.0f, Easing::StepEnd);

    tween.update(0.5f);
    CHECK(tween.value() == doctest::Approx(0.0f));

    tween.update(0.5f);
    CHECK(tween.value() == doctest::Approx(10.0f));
}
