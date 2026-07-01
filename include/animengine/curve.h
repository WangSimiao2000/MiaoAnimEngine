#pragma once

#include <vector>

#include "easing.h"

namespace animengine {

/**
 * How a segment leaving a keyframe is interpolated.
 *
 * The mode is owned by the *left* keyframe of a segment: a keyframe's interp
 * governs the half-open interval [thisKeyframe.time, nextKeyframe.time). The
 * last keyframe's interp is therefore never read (it has no segment to its
 * right) -- this is intentional, see addKeyframe().
 *
 * TODO(curve): This per-keyframe enum is the most basic model. High-end
 * engines (Unity AnimationCurve, Unreal FRichCurve, Blender F-Curves, glTF
 * CUBICSPLINE) instead store per-keyframe in/out tangents and evaluate with a
 * cubic Hermite/Bezier spline, plus a tangent mode (Auto/Flat/Free/Weighted).
 * When we need editable curve handles or C1-continuous smoothing, upgrade to:
 *   - add Cubic to this enum,
 *   - add inTangent/outTangent (and optionally weights) to Keyframe,
 *   - add a hermite(p0, m0, p1, m1, t) primitive in interpolation.h,
 *   - recompute Auto tangents from neighbours after addKeyframe.
 */

struct Keyframe {
    float time{};
    float value{};
    Easing easing = Easing::Linear;
};

class Curve {
public:
    void addKeyframe(float time, float value, Easing easing = Easing::Linear);
    [[nodiscard]] float evaluate(float time) const;

private:
    std::vector<Keyframe> _keyframes;
};

}  // namespace animengine