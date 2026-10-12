#include "MetalClipSpace.h"

namespace ios_renderer {

Matrix4 toMetalClipSpace(const Matrix4& projection) {
    Matrix4 result = projection;
    for (std::size_t column = 0; column < 4; ++column) {
        const std::size_t base = column * 4;
        result[base + 2] = 0.5f * (projection[base + 2] + projection[base + 3]);
    }
    return result;
}

}
