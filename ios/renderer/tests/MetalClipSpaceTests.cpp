#include "../MetalClipSpace.h"

#include <cassert>
#include <cmath>

using ios_renderer::Matrix4;
using ios_renderer::toMetalClipSpace;

namespace {
bool near(float a, float b) { return std::fabs(a - b) < 0.0001f; }

void testIdentityClipCorrection() {
    const Matrix4 identity = ios_renderer::MatrixStacks::identity();
    const Matrix4 metal = toMetalClipSpace(identity);
    assert(near(metal[10], 0.5f));
    assert(near(metal[14], 0.5f));
    assert(near(metal[15], 1.0f));
    assert(near(metal[0], 1.0f));
    assert(near(metal[5], 1.0f));
}

void testOpenGLDepthRangeMapsToMetalDepthRange() {
    Matrix4 projection{};
    projection[0] = 1.0f;
    projection[5] = 1.0f;
    projection[10] = -11.0f / 9.0f;
    projection[11] = -1.0f;
    projection[14] = -20.0f / 9.0f;

    const Matrix4 metal = toMetalClipSpace(projection);
    assert(near(metal[10], -10.0f / 9.0f));
    assert(near(metal[14], -10.0f / 9.0f));
    assert(near(metal[11], -1.0f));
    assert(near(metal[15], 0.0f));
}
}

int main() {
    testIdentityClipCorrection();
    testOpenGLDepthRangeMapsToMetalDepthRange();
    return 0;
}
