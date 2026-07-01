#pragma once

namespace animengine {

class Spring {
public:
    Spring(float duration, float bounce, float initialValue = 0.0f, float initialVelocity = 0.0f);
    void update(float dt);
    void setTarget(float target);
    float value() const;
    float velocity() const;
    bool isSettled() const;
    void snapTo(float value);

private:
    float _stiffness;
    float _damping;
    float _value;
    float _velocity;
    float _target;
    float _restDisplacementThreshold;
    float _restVelocityThreshold;
};
}  // namespace animengine