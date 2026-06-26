#include "animengine/interpolation.h"

namespace animengine {
float linear(float start, float end, float t) {
    return start + t * (end - start);
}
}  // namespace animengine
