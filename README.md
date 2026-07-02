# AnimEngine

AnimEngine is a lightweight C++ runtime focused on real-time animation systems.

See [AnimEngineDemo](https://github.com/WangSimiao2000/AnimEngineDemo) for runnable examples.

## Features

### Implemented

- Keyframe Interpolation
- Curve Evaluation
- Easing Functions (StepStart, StepEnd, Linear, EaseIn, EaseOut, EaseInOut)
- Tween (single-value from→to over a duration, with easing)
- Spring (physics-based, interruptible / re-targetable with velocity continuity)

### Planned

- Scene Graph
- Transform Hierarchy
- Timeline
- Animation Clip
- Blend Tree
- Animation Layers
- State Machine
- Constraints
- Skeletal Animation

## API Proposal

Simplified animation API for common use cases:

```cpp
// Current: manually add keyframes
curve.addKeyframe(0.0f, 0.0f, Easing::EaseIn);
curve.addKeyframe(2.0f, 100.0f, Easing::EaseIn);

// Proposed: convenience method
curve.animate(0.0f, 100.0f, Easing::EaseIn, 2.0f);  // from, to, easing, duration
```

## Building

### Requirements

- CMake 3.16 or higher
- C++17 compiler

### Standalone Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Running Tests

```bash
cmake .. -DANIMENGINE_BUILD_TESTS=ON
cmake --build .
ctest
```

### Integration

Add to your CMake project via `add_subdirectory`:

```cmake
add_subdirectory(external/AnimEngine)
target_link_libraries(your_target PRIVATE AnimEngine::animengine)
```