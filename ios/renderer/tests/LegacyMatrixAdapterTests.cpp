#include "../LegacyMatrixAdapter.h"

#include <cassert>
#include <cmath>
#include <stdexcept>

using ios_renderer::LegacyMatrixAdapter;

namespace {
bool near(float a, float b) { return std::fabs(a - b) < 0.0001f; }

void testLegacyMatrixCalls() {
    LegacyMatrixAdapter renderer;
    renderer.MatrixMode(0);
    renderer.MatrixSetIdentity();
    renderer.MatrixTranslate(1.0f, 2.0f, 3.0f);
    assert(near(renderer.MatrixGet(0)[12], 1.0f));
    assert(near(renderer.MatrixGet(0)[13], 2.0f));
    assert(near(renderer.MatrixGet(0)[14], 3.0f));

    renderer.MatrixPush();
    renderer.MatrixScale(2.0f, 2.0f, 2.0f);
    assert(near(renderer.MatrixGet(0)[0], 2.0f));
    renderer.MatrixPop();
    assert(near(renderer.MatrixGet(0)[0], 1.0f));
}

void testProjectionAndModeValidation() {
    LegacyMatrixAdapter renderer;
    renderer.MatrixMode(1);
    renderer.MatrixPerspective(90.0f, 1.0f, 0.1f, 100.0f);
    assert(near(renderer.MatrixGet(1)[11], -1.0f));

    bool threw = false;
    try {
        renderer.MatrixMode(99);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
}
}

int main() {
    testLegacyMatrixCalls();
    testProjectionAndModeValidation();
    return 0;
}
