#pragma once

#include <cmath>
#include <stdexcept>

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

namespace detail {

[[nodiscard]] inline float scaleBy(float value, double scale) {
    return static_cast<float>(static_cast<double>(value) * scale);
}

[[nodiscard]] inline Vec3 scaleBy(const Vec3& value, double scale) {
    return {
        static_cast<float>(static_cast<double>(value.x) * scale),
        static_cast<float>(static_cast<double>(value.y) * scale),
        static_cast<float>(static_cast<double>(value.z) * scale),
    };
}

[[nodiscard]] inline Color scaleBy(const Color& value, double scale) {
    return {
        static_cast<float>(static_cast<double>(value.r) * scale),
        static_cast<float>(static_cast<double>(value.g) * scale),
        static_cast<float>(static_cast<double>(value.b) * scale),
        static_cast<float>(static_cast<double>(value.a) * scale),
    };
}

template <class T>
[[nodiscard]] auto scaleBy(const T& value, double scale)
    -> decltype(value * static_cast<float>(scale)) {
    return value * static_cast<float>(scale);
}

template <class T>
[[nodiscard]] T zeroLike(const T& value) {
    return value - value;
}

}  // namespace detail

template <class T = float>
class Spring {
public:
    Spring(float duration, float bounce, T initialValue = T{})
        : Spring(duration, bounce, initialValue, detail::zeroLike(initialValue)) {}

    Spring(float duration, float bounce, T initialValue, T initialVelocity)
        : _angularFrequency(0.0),
          _dampingRatio(0.0),
          _value(initialValue),
          _velocity(initialVelocity),
          _target(initialValue),
          _restDisplacementThreshold(0.01f),
          _restVelocityThreshold(0.01f) {
        if (!std::isfinite(duration) || duration <= 0.0f) {
            throw std::invalid_argument("Spring duration must be finite and greater than zero");
        }
        if (!std::isfinite(bounce) || bounce <= -1.0f || bounce >= 1.0f) {
            throw std::invalid_argument("Spring bounce must be finite and in (-1, 1)");
        }

        constexpr double pi = 3.14159265358979323846;
        _dampingRatio = bounce >= 0.0f ? 1.0 - bounce : 1.0 / (1.0 + bounce);
        _angularFrequency = 2.0 * pi / duration;
    }

    void update(float dt) {
        if (!std::isfinite(dt) || dt < 0.0f) {
            throw std::invalid_argument("Spring delta time must be finite and non-negative");
        }
        if (dt == 0.0f) {
            return;
        }

        const double time = dt;
        const double omega = _angularFrequency;
        const double zeta = _dampingRatio;
        double positionFromPosition;
        double positionFromVelocity;
        double velocityFromPosition;
        double velocityFromVelocity;

        if (zeta < 1.0) {
            const double decayRate = zeta * omega;
            const double dampedFrequency = omega * std::sqrt(1.0 - zeta * zeta);
            const double angle = dampedFrequency * time;
            const double decay = std::exp(-decayRate * time);
            const double sine = std::sin(angle);
            const double cosine = std::cos(angle);
            const double decayToFrequency = decayRate / dampedFrequency;

            positionFromPosition = decay * (cosine + decayToFrequency * sine);
            positionFromVelocity = decay * sine / dampedFrequency;
            velocityFromPosition = -decay * omega * omega * sine / dampedFrequency;
            velocityFromVelocity = decay * (cosine - decayToFrequency * sine);
        } else if (zeta > 1.0) {
            const double root = std::sqrt(zeta * zeta - 1.0);
            const double rootSum = zeta + root;
            const double slowRoot = -omega / rootSum;
            const double fastRoot = -omega * rootSum;
            const double rootDifference = slowRoot - fastRoot;
            const double slowDecay = std::exp(slowRoot * time);
            const double fastDecay = std::exp(fastRoot * time);

            positionFromPosition = (-fastRoot * slowDecay + slowRoot * fastDecay) / rootDifference;
            positionFromVelocity = (slowDecay - fastDecay) / rootDifference;
            velocityFromPosition = slowRoot * fastRoot * (fastDecay - slowDecay) / rootDifference;
            velocityFromVelocity = (slowRoot * slowDecay - fastRoot * fastDecay) / rootDifference;
        } else {
            const double scaledTime = omega * time;
            const double decay = std::exp(-scaledTime);

            positionFromPosition = decay * (1.0 + scaledTime);
            positionFromVelocity = decay * time;
            velocityFromPosition = -decay * omega * scaledTime;
            velocityFromVelocity = decay * (1.0 - scaledTime);
        }

        const T displacement = _value - _target;
        const T initialVelocity = _velocity;
        _value = _target + detail::scaleBy(displacement, positionFromPosition) +
                 detail::scaleBy(initialVelocity, positionFromVelocity);
        _velocity = detail::scaleBy(displacement, velocityFromPosition) +
                    detail::scaleBy(initialVelocity, velocityFromVelocity);
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
        _target = value;
        _velocity = detail::zeroLike(value);
    }

private:
    double _angularFrequency;
    double _dampingRatio;
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