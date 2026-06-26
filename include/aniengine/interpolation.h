#pragma once

namespace aniengine {
/**
 * Linearly interpolates between two values.
 *
 * Computes start + t * (end - start). With t in [0, 1] the result moves
 * uniformly from start to end; values outside [0, 1] extrapolate.
 *
 * @param start The value returned when t = 0.
 * @param end   The value returned when t = 1.
 * @param t     The interpolation factor, normally in the range [0, 1].
 * @return The interpolated value between start and end.
 */
float linear(float start, float end, float t);

}  // namespace aniengine
