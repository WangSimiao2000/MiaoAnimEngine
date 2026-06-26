#pragma once

#include <vector>

namespace animengine {

struct Keyframe {
    float time;
    float value;
};

class Curve {
public:
    /**
     * Adds a keyframe, keeping keyframes sorted by ascending time.
     * If one already exists at the same time, its value is overwritten.
     *
     * @param time  The keyframe's time.
     * @param value The value at that time.
     */
    void addKeyframe(float time, float value);

    /**
     * Returns the value at the given time.
     *
     * Outside the keyframe range the first/last value is clamped (both ends
     * closed). Inside, each segment is the half-open interval
     * [prev.time, next.time): a time landing exactly on a keyframe takes that
     * keyframe's value. Adjacent keyframes are interpolated linearly. Returns
     * 0 if the curve is empty.
     *
     * @param time The time to evaluate at.
     * @return The interpolated (or clamped) value.
     */
    float evaluate(float time) const;

private:
    std::vector<Keyframe> _keyframes;
};

}  // namespace animengine