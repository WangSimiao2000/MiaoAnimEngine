#pragma once

#include "math.h"

namespace animengine {

[[nodiscard]] inline float magnitudeSquared(float v) {
    return v * v;
}

[[nodiscard]] inline float magnitudeSquared(const Vec3& v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

[[nodiscard]] inline float magnitudeSquared(const Color& c) {
    return c.r * c.r + c.g * c.g + c.b * c.b + c.a * c.a;
}

template <class T = float>
class Spring {
public:
    Spring(float duration, float bounce, T initialValue = T{}, T initialVelocity = T{})
        : _stiffness(0.0f),
          _damping(0.0f),
          _value(initialValue),
          _velocity(initialVelocity),
          _target(initialValue),
          _restDisplacementThreshold(0.01f),
          _restVelocityThreshold(0.01f) {
        constexpr float pi = 3.14159265f;
        const float zeta = bounce >= 0 ? (1 - bounce) : 1 / (1 + bounce);
        const float omega = 2 * pi / duration;

        _stiffness = omega * omega;
        _damping = 2 * zeta * omega;
    }

    void update(float dt) {
        const T acceleration = (_target - _value) * _stiffness - _velocity * _damping;
        _velocity = _velocity + acceleration * dt;
        _value = _value + _velocity * dt;
    }

    void setTarget(T target) {
        _target = target;
    }

    [[nodiscard]] T value() const {
        return _value;
    }

    [[nodiscard]] T velocity() const {
        return _velocity;
    }

    [[nodiscard]] bool isSettled() const {
        return magnitudeSquared(_value - _target) <
                   _restDisplacementThreshold * _restDisplacementThreshold &&
               magnitudeSquared(_velocity) < _restVelocityThreshold * _restVelocityThreshold;
    }

    void snapTo(T value) {
        _value = value;
        _velocity = T{};
    }

private:
    float _stiffness;
    float _damping;
    T _value;
    T _velocity;
    T _target;
    float _restDisplacementThreshold;
    float _restVelocityThreshold;
};

using SpringFloat = Spring<float>;
using SpringVec3 = Spring<Vec3>;
using SpringColor = Spring<Color>;

}  // namespace animengine