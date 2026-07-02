#include "animengine/math.h"

#include <cmath>

namespace animengine {

namespace {

Quat normalize(Quat q) {
    const float n = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    if (n <= 0.0f) {
        return Quat{0.0f, 0.0f, 0.0f, 1.0f};
    }
    const float inv = 1.0f / n;
    return Quat{q.x * inv, q.y * inv, q.z * inv, q.w * inv};
}

}  // namespace

Quat lerp(const Quat& a, Quat b, const float t) {
    float dot = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    if (dot < 0.0f) {
        b = Quat{-b.x, -b.y, -b.z, -b.w};
        dot = -dot;
    }

    if (dot > 0.9995f) {
        return normalize(Quat{a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t,
                              a.w + (b.w - a.w) * t});
    }

    const float theta0 = std::acos(dot);
    const float theta = theta0 * t;
    const float sinTheta0 = std::sin(theta0);
    const float s0 = std::sin(theta0 - theta) / sinTheta0;
    const float s1 = std::sin(theta) / sinTheta0;

    return Quat{a.x * s0 + b.x * s1, a.y * s0 + b.y * s1, a.z * s0 + b.z * s1, a.w * s0 + b.w * s1};
}

}  // namespace animengine
