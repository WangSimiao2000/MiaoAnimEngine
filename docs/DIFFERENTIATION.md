# Differentiation Strategy

Where should AnimEngine focus to be *distinctive*, rather than yet another
DOTween/GSAP clone? Competing on raw feature breadth against decade-old,
ecosystem-locked incumbents is a losing battle for a small project. The edge has
to come from an underserved angle where the project's constraints
(C++, built from scratch, render-agnostic core, strong test/doc discipline) are
actually advantages.

## Direction A — Spring physics + interruptible by default (recommended)

**The gap.** Almost every C++ tween library uses the "duration + easing" model
(the DOTween lineage). Modern UI motion (iOS, SwiftUI, Framer Motion, React
Spring) has moved to **spring physics with mid-flight interruption and velocity
continuity**: when the target changes, the animation does not restart — it
carries its current velocity and smoothly re-targets. This is barely done well
anywhere in the C++ world.

- One-line pitch: *"Motion that responds naturally to interruption, like native
  iOS — in portable C++."*
- Core tech: critically/under-damped spring integrators, preserved velocity
  state, velocity inheritance on re-target.
- Our advantage: the raylib demo is the perfect harness to *show* the
  interruption feel — click a target, the cube chases it with momentum, change
  the target mid-flight and it never restarts. The difference from a plain tween
  is visible at a glance.

Modern, demoable, technically deep, and a genuine gap in C++. Strongest bet.

## Direction B — Zero-dependency embeddable core ("the SQLite of animation")

**The gap.** Mainstream libraries are ecosystem-bound: GSAP→DOM, DOTween→Unity,
Lottie→After Effects. A genuinely engine-agnostic, dependency-free C++ animation
core that drops into *any* engine or UI toolkit is comparatively rare.

The current architecture already points this way: the core has no raylib
dependency; raylib lives only in the demo. Turn that into explicit positioning:

- Single-header or very few files, no third-party deps, optional C ABI so any
  engine/UI toolkit can embed it.
- One-line pitch: *"Tiny, embeddable, dependency-free animation core."*
- Risk: the positioning itself is not flashy; it wins on ergonomics and docs —
  which happens to match this project's documentation/testing habits.

Stacks well with A: an embeddable core *plus* spring dynamics is a strong combo.

## Direction C — Deterministic animation (for netcode / replays)

**The gap.** Lockstep multiplayer and replay systems require **cross-platform
frame-deterministic** behaviour. Floating point differs across platforms, and
most animation libraries make no determinism guarantee. A deterministic
(fixed-point or strictly controlled float) animation/sampling engine is a real
pain point for networked-game developers.

- One-line pitch: *"Deterministic animation sampling for rollback netcode and
  replays."*
- Our advantage: a strong test culture is exactly the moat here — determinism
  has to be backed by extensive regression tests.
- Risk: narrow audience (a subset of multiplayer developers), but narrow-and-deep
  is itself a form of differentiation.

## Direction D (cross-cutting) — Legible, visual, well-documented engine

The project already has a demo harness and good documentation habits. Lean into
being *the most readable, visually explained, best-documented* animation engine
— simultaneously a usable library and a learning resource. Not a standalone
direction, but a "personality" that amplifies whichever of A/B/C is chosen and
helps build a community following.

## Recommendation

**Lead with A (spring + interruptible), backed by B's architectural discipline,
wrapped in D's personality.**

- A combines technical depth, a modern trend, and strong demoability — and the
  C++ ecosystem genuinely lacks it.
- B is almost free to claim (the architecture is already right).
- D amplifies existing strengths.
- C is compelling but its audience is narrow; pursue it only if networking /
  rollback is already a personal interest.

### First milestone if choosing A

**Status: achieved.** Steps 1 and 2 below are done — `Spring` ships as an
interruptible peer to `Tween`, and the raylib demo shows the Tween-vs-Spring
re-targeting contrast side by side.

1. Add a `Spring` integrator (stiffness / damping / mass plus current
   value/velocity state), as a peer to `Tween`. ✅ *(mass not yet exposed)*
2. Add a demo scene: click a target point, the cube chases it with momentum, and
   changing the target mid-flight does **not** restart the motion. ✅ *(SPACE
   flips the shared target; RED Tween restarts, BLUE Spring keeps momentum)*
3. Once this works, the engine's character is already visibly different from
   off-the-shelf C++ tween libraries. ✅
