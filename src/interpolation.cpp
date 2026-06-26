#include "aniengine/interpolation.h"

namespace aniengine {
float linear(float start, float end, float t) {
    return start + t * (end - start);
}
}  // namespace aniengine
