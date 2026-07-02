# AnimEngine Roadmap

This document tracks where AnimEngine stands today and what is still missing to
make it a production-grade animation engine usable in UI frameworks and game
engines (think DOTween, GSAP, Framer Motion, SwiftUI animations).

## Current state

AnimEngine is currently an **easing-math + single-value tween/spring** library
— a solid foundation, but only the lowest layer of a real animation engine.

Implemented:

- **Easing**: 6 functions — `StepStart`, `StepEnd`, `Linear`, and quadratic
  `EaseIn` / `EaseOut` / `EaseInOut`.
- **Curve**: keyframe-based curve. Keyframes are kept sorted by time, values are
  clamped outside the keyframe range, and each segment is interpolated according
  to its *left* keyframe's easing mode (linear / step today).
- **Tween**: a single `float` from→to over a duration, with easing,
  `update(dt)`, `reset()`, `value()`, `elapsed()`, `duration()`, `isFinished()`.
- **Spring**: physics-based single-`float` animation (stiffness/damping derived
  from a duration + bounce), **interruptible** — `setTarget()` mid-flight keeps
  the current velocity and smoothly redirects. `update(dt)`, `value()`,
  `velocity()`, `isSettled()`, `snapTo()`.
- **Tooling**: doctest-based unit tests with good coverage, clang-format config,
  and CI (format check + build).

Capability boundary: the engine can only drive a **single scalar**, in a
**single animation**, with the **caller manually reading the value and applying
it**, and **manually managing each tween's lifecycle**.

Rough completion estimate against a full-featured engine: **~25–30%**. The
foundation (math, single-value tween, an interruptible spring, test/build
tooling) is solid; the "engine" layers (sequencing, playback control,
multi-type values, target binding, a central manager) are essentially empty.

## Gap analysis

### Tier 1 — Core gaps (without these it is a math library, not an engine)

1. **Multi-value type support.** Only `float` today. UI/games need `vec2`,
   `vec3`, `vec4`, **color** (with correct color-space interpolation),
   **quaternion rotation** (slerp, not linear lerp), int, and bool. Requires a
   generic / templated value type with per-type `lerp` specializations. Largest
   surface-area gap.

2. **Timeline / sequencing.** Today there is only a single tween. Real use
   almost always composes animations:
   - **Sequence** (play B after A finishes)
   - **Parallel / group** (play a set together)
   - **Delay** and **stagger** (offset start times)
   - **Nested timelines**

   This is the heart of GSAP/DOTween.

3. **Playback control.** Tween only has `update` / `reset` / `isFinished`.
   Missing: **pause/resume**, **seek** (jump to an arbitrary time), **playback
   speed (timescale)**, **reverse playback**, and **loop modes**
   (restart / yoyo / ping-pong / finite count / infinite). UI animation almost
   always needs loop and yoyo.

4. **Callbacks / events.** `onStart` / `onUpdate` / `onComplete` / `onLoop`.
   "Run logic after the animation ends" is a baseline UI requirement and is
   completely absent today.

### Tier 2 — Usability (decides how pleasant it is to use)

5. **Target binding / automatic application.** Today the caller must manually do
   `obj.y = tween.value()`. Mature engines bind to a property (setter/getter or
   reference) so the tween writes back directly, removing per-property
   boilerplate.

6. **Central manager / update loop.** A manager that owns all active animations,
   updates them in one `update(dt)`, and auto-recycles finished ones. Today the
   caller must store, update, and destroy every tween by hand, which does not
   scale.

7. **Time management.** Global timescale, unscaled time (UI must keep animating
   while the game is paused), and fixed vs. variable timestep handling.

### Tier 3 — Advanced / modern (decides whether it can match top-tier engines)

8. **Curve interpolation upgrade.** Upgrade from linear/step to **cubic
   Hermite/Bezier tangents** (matching Unity `AnimationCurve`, glTF
   `CUBICSPLINE`), with tangent modes (auto / flat / free / weighted) for
   C1-continuous, editable curves. Already noted as a TODO in `curve.h`.

9. **Spring / physics-based animation.** ✅ **Implemented** (see `spring.h`).
   SwiftUI, React Spring, and Framer Motion all treat spring dynamics
   (stiffness / damping / mass) as the default UI motion model. AnimEngine's
   `Spring` is **interruptible / re-targetable** — when the target changes
   mid-flight, velocity is preserved for a smooth transition. Remaining
   follow-ups: expose an explicit `mass` parameter, and generalize the spring to
   the multi-value types from gap #1.

10. **Easing function completeness.** The full Penner set (~30+:
    cubic / quart / quint / sine / expo / circ / back / elastic / bounce, each in
    in/out/inout) plus **cubic-bezier with arbitrary control points** (CSS
    style) plus user-supplied custom functions. Only quadratic exists today.

11. **Data-driven / serialization.** Load animation definitions from JSON/binary
    so designers can author without code (Lottie/Rive direction), ideally with a
    curve-editor integration.

12. **Performance engineering.** Object pooling (avoid per-frame allocation),
    batched updates, and cache-friendly (SoA) storage. Matters most for game
    engines.

## Suggested order of work

If the goal is "genuinely usable in UI and game engines":

1. **Multi-value types** — templated Tween/Curve + lerp/slerp for vec/color/quat
   → opens up the usable surface.
2. **Playback control + callbacks** — loop/yoyo/pause/seek + onComplete
   → makes a single animation pleasant.
3. **Timeline sequencing** — sequence/parallel/delay/stagger
   → upgrades from "tween" to "engine".
4. **Manager + target binding** — central update + property write-back
   → makes it production-usable.
5. ~~**Spring + interruptible**~~ (done) and **cubic curves** (the `curve.h`
   TODO) → matches modern top-tier engines.
6. **Data-driven / performance** → productization.
