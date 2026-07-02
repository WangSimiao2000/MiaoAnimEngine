# AnimEngine Roadmap

This document tracks where AnimEngine stands today and what is still missing to
make it a production-grade animation engine usable in UI frameworks and game
engines (think DOTween, GSAP, Framer Motion, SwiftUI animations).

## Current state

AnimEngine is currently an **easing-math + multi-value tween/curve/spring**
library — a solid foundation, but still only the lower layers of a real
animation engine.

Implemented:

- **Easing**: 6 functions — `StepStart`, `StepEnd`, `Linear`, and quadratic
  `EaseIn` / `EaseOut` / `EaseInOut`. The reshaping is split from interpolation
  (`easeShape` in `easing.h`, `lerp`/`ease` in `interpolation.h`).
- **Value types + interpolation**: built-in `Vec3`, `Color`, `Quat`, plus a
  generic `lerp<T>` customization point (default component-wise; `Quat`
  overloaded to **slerp** — shortest-arc, unit-length-preserving).
- **Curve**: `Curve<T>` keyframe-based curve. Keyframes are kept sorted by time,
  values are clamped outside the keyframe range, and each segment is
  interpolated according to its *left* keyframe's easing mode (linear / step).
- **Tween**: `Tween<T>` — a single value from→to over a duration, with easing,
  `update(dt)`, `reset()`, `value()`, `elapsed()`, `duration()`, `isFinished()`.
- **Spring**: `Spring<T>` — physics-based (stiffness/damping derived from a
  duration + bounce), **interruptible** — `setTarget()` mid-flight keeps the
  current velocity and smoothly redirects. `update(dt)`, `value()`, `velocity()`,
  `isSettled()`, `snapTo()`. Works for scalars and vectors; `Quat` is excluded
  (rotation springs need an angular-velocity formulation).
- **Tooling**: doctest-based unit tests with good coverage, clang-format config,
  and CI (format check + build).

All of `Tween` / `Curve` / `Spring` are templated over the value type, so they
drive `float`, `Vec3`, `Color`, and (for Tween/Curve) `Quat` — with `float` the
default via CTAD and convenience aliases (`TweenFloat`, `CurveVec3`, …).

Capability boundary: the engine drives **one value** per animation, in a
**single animation**, with the **caller manually reading the value and applying
it**, and **manually managing each animation's lifecycle**. Multi-value *types*
are done; the missing "engine" layers are sequencing, playback control, target
binding, and a central manager.

Rough completion estimate against a full-featured engine: **~35%**. The
foundation (easing math, multi-value tween/curve, an interruptible spring,
slerp, test/build tooling) is solid; the composition/playback/management layers
are still empty.

## Gap analysis

### Tier 1 — Core gaps (without these it is a math library, not an engine)

1. **Multi-value type support.** ✅ **Implemented.** `Tween` / `Curve` / `Spring`
   are templated over the value type, with a generic `lerp<T>` customization
   point. Built-in `Vec3`, `Color`, `Quat` ship today, and `Quat` interpolates
   via **slerp** (shortest-arc, unit-length-preserving). Users can plug their own
   type by overloading `lerp` (and `magnitudeSquared` for `Spring`). Remaining
   follow-ups: more built-ins (`vec2`/`vec4`/`int`/`bool`) and correct
   color-space interpolation (currently linear per-channel, see the `Color` TODO).

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
   mid-flight, velocity is preserved for a smooth transition. Now generalized to
   the multi-value types (`Spring<Vec3>` etc.; `Quat` excluded). Remaining
   follow-up (optional): a `fromCoefficients(stiffness, damping, mass)` entry for
   Unreal/react-spring-style physical parameterization — mass only has an effect
   there, not in the duration/bounce model (SwiftUI/Flutter fix mass at 1).

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

1. ~~**Multi-value types**~~ (done) — templated Tween/Curve/Spring + lerp/slerp
   for vec/color/quat.
2. **Playback control + callbacks** — loop/yoyo/pause/seek + onComplete
   → makes a single animation pleasant.
3. **Timeline sequencing** — sequence/parallel/delay/stagger
   → upgrades from "tween" to "engine".
4. **Manager + target binding** — central update + property write-back
   → makes it production-usable.
5. ~~**Spring + interruptible**~~ (done) and **cubic curves** (the `curve.h`
   TODO) → matches modern top-tier engines.
6. **Data-driven / performance** → productization.
