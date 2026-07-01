#include "animengine/spring.h"

#include <cmath>

namespace animengine {

constexpr float pi = 3.14159265f;

Spring::Spring(const float duration, const float bounce, const float initialValue,
               const float initialVelocity)
    : _stiffness(0.0f),
      _damping(0.0f),
      _value(initialValue),
      _velocity(initialVelocity),
      _target(initialValue),
      _restDisplacementThreshold(0.01f),
      _restVelocityThreshold(0.01f) {
    const float zeta = bounce >= 0 ? (1 - bounce) : 1 / (1 + bounce);
    const float omega = 2 * pi / duration;

    _stiffness = omega * omega;
    _damping = 2 * zeta * omega;
}

void Spring::update(const float dt) {
    const float acceleration = -_stiffness * (_value - _target) - _damping * _velocity;
    _velocity += acceleration * dt;
    _value += _velocity * dt;
}

void Spring::setTarget(const float target) {
    _target = target;
}

float Spring::value() const {
    return _value;
}

float Spring::velocity() const {
    return _velocity;
}

bool Spring::isSettled() const {
    return std::fabs(_value - _target) < _restDisplacementThreshold &&
           std::fabs(_velocity) < _restVelocityThreshold;
}

void Spring::snapTo(const float value) {
    _value = value;
    _velocity = 0.0f;
}

}  // namespace animengine