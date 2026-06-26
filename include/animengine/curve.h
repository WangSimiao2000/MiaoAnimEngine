#pragma once

#include <vector>

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
enum class Interpolation {
    Step,    // Hold prev value until the next keyframe (no interpolation).
    Linear,  // Straight line between adjacent keyframes.
    // TODO(curve): Smooth (smoothstep), Cubic (Hermite w/ tangents), ...
};

struct Keyframe {
    float time;
    float value;
    Interpolation interp = Interpolation::Linear;
};

class Curve {
public:
    /**
     * Adds a keyframe, keeping keyframes sorted by ascending time.
     * If one already exists at the same time, its value (and interp) is
     * overwritten.
     *
     * @param time   The keyframe's time.
     * @param value  The value at that time.
     * @param interp How to interpolate the segment leaving this keyframe.
     *               Ignored for the last keyframe (no segment to its right),
     *               so it is safe to omit (defaults to Linear) on the final
     *               point; the stored value is simply never read.
     */
    void addKeyframe(float time, float value, Interpolation interp = Interpolation::Linear);

    /**
     * Returns the value at the given time.
     *
     * Outside the keyframe range the first/last value is clamped (both ends
     * closed). Inside, each segment is the half-open interval
     * [prev.time, next.time): a time landing exactly on a keyframe takes that
     * keyframe's value. The segment is interpolated according to the left
     * keyframe's interp (Step holds prev's value, Linear interpolates).
     * Returns 0 if the curve is empty.
     *
     * @param time The time to evaluate at.
     * @return The interpolated (or clamped) value.
     */
    float evaluate(float time) const;

private:
    std::vector<Keyframe> _keyframes;
};

}  // namespace animengine