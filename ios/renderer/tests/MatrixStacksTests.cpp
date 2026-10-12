#include "../MatrixStacks.h"

#include <cassert>
#include <cmath>
#include <stdexcept>

using ios_renderer::Matrix4;
using ios_renderer::MatrixMode;
using ios_renderer::MatrixStacks;

namespace {

bool near(float a, float b) { return std::fabs(a - b) < 0.0001f; }

void testIdentityAndTranslate() {
    MatrixStacks matrices;
    assert(matrices.current() == MatrixStacks::identity());
    matrices.translate(2.0f, -3.0f, 4.0f);
    const Matrix4& m = matrices.current();
    assert(near(m[12], 2.0f));
    assert(near(m[13], -3.0f));
    assert(near(m[14], 4.0f));
}

void testIndependentStacksAndPushPop() {
    MatrixStacks matrices;
    matrices.translate(5.0f, 0.0f, 0.0f);
    matrices.push();
    matrices.translate(2.0f, 0.0f, 0.0f);
    assert(near(matrices.current()[12], 7.0f));
    assert(matrices.pop());
    assert(near(matrices.current()[12], 5.0f));
    assert(!matrices.pop());

    matrices.setMode(MatrixMode::Projection);
    assert(matrices.current() == MatrixStacks::identity());
    matrices.translate(0.0f, 3.0f, 0.0f);
    matrices.setMode(MatrixMode::ModelView);
    assert(near(matrices.current()[12], 5.0f));
}

void testPerspectiveAndOrthographic() {
    MatrixStacks matrices;
    matrices.setMode(MatrixMode::Projection);
    matrices.perspective(90.0f, 2.0f, 1.0f, 11.0f);
    const Matrix4 p = matrices.current();
    assert(near(p[0], 0.5f));
    assert(near(p[5], 1.0f));
    assert(near(p[11], -1.0f));

    matrices.loadIdentity();
    matrices.orthographic(-2.0f, 2.0f, -1.0f, 1.0f, 1.0f, 11.0f);
    const Matrix4 o = matrices.current();
    assert(near(o[0], 0.5f));
    assert(near(o[5], 1.0f));
    assert(near(o[10], -0.2f));
}

void testInvalidProjection() {
    MatrixStacks matrices;
    bool threw = false;
    try {
        matrices.perspective(90.0f, 0.0f, 1.0f, 10.0f);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
}

}

int main() {
    testIdentityAndTranslate();
    testIndependentStacksAndPushPop();
    testPerspectiveAndOrthographic();
    testInvalidProjection();
    return 0;
}
